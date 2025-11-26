#include "UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UUBTTask_DefaultAtk* UUBTTask_DefaultAtk::Instance;

UUBTTask_DefaultAtk::UUBTTask_DefaultAtk()
{
    NodeName = TEXT("NoStatusAttack");
    bNotifyTick = true;
    bCreateNodeInstance = true;
}

UUBTTask_DefaultAtk* UUBTTask_DefaultAtk::GetInstance()
{
	return Instance;
}

EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	Instance = this;
	
	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Self = Cast<AMob>(BlackBoard->GetValueAsObject("SelfActor"));
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	checkf(Controller && Self && Target, TEXT("Invalid references in DefaultAtk task"));

	CurrentMob = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
	checkf(CurrentMob, TEXT("CurrentMob is invalid at ExecuteTask"));

	CurrentMob->SetTreeId(0);
	CurrentMob->SetIsAttacked(false);
	BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
	BlackBoard->SetValueAsBool("Attacked?", CurrentMob->GetIsIsAttacked());
	
	bWaitingForThinkCompletion = true;
	bBusy = true;

	DecisionMaker.Clear();
	DecisionMaker.Setup(CurrentMob);
	Decision = DecisionMaker.Thought();

	Target->GetBattleHUD()->DecisionDisplayer->Show();
	Target->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
		FText::FromString(Self->GetData()->EnemyName.ToString() + " is thinking...")
	);
	
	Controller->GetWorld()->GetTimerManager().ClearTimer(DelayHandle);

	if (Delay > 0.f)
	{
		Controller->GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&UUBTTask_DefaultAtk::OnThinkComplete_Internal,
			Delay,
			false
		);
	}
	else
	{
		OnThinkComplete_Internal();
	}

	TreeComp = &OwnerComp;
	return EBTNodeResult::InProgress;
}

void UUBTTask_DefaultAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!bWaitingForThinkCompletion || !bBusy)
        return;

    AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
    if (!Controller || !CurrentMob || !BlackBoard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // Wait until minigame is fully finished
    if (!CurrentMob->IsMinigameStarted() || !CurrentMob->IsMinigameEnded() || !Target->GetMinigameHandler()->IsPlayerMinigameEnded())
        return;

    // Minigame finished: cleanup and finish task
    CurrentMob->SetTreeId(-1);
    CurrentMob->SetIsBuffedAtk(false);
    BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
    BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
    BlackBoard->SetValueAsBool("IsDefenceDebuffed?", CurrentMob->GetIsTargetDefenceDebuffed());

    CurrentMob->GetBattleHandler()->GetBattleInfo()->ClearInfo();

    bBusy = false;
    bWaitingForThinkCompletion = false;

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UUBTTask_DefaultAtk::OnThinkComplete(UBehaviorTreeComponent* OwnerComp, AICC_AIController* Controller)
{
    if (!OwnerComp || !Controller || !CurrentMob || !BlackBoard)
        return;

    bWaitingForThinkCompletion = true;

    AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
    if (!Target)
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // Early-outs for ashamed or not-current-turn
    if (CurrentMob->IsAshamed() || CurrentMob != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
    {
        if (CurrentMob->IsAshamed())
        {
            CurrentMob->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(CurrentMob->GetData()->EnemyName.ToString() + " skipped the turn (Ashamed)"));
            DebugHelper::AddMessageToLog(CurrentMob->GetData()->EnemyName.ToString() + " skipped the turn (Ashamed)");

            CurrentMob->SetTreeId(-1);
            CurrentMob->SetIsBuffedAtk(false);
            BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
            BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
            BlackBoard->SetValueAsBool("IsDefenceDebuffed?", CurrentMob->GetIsTargetDefenceDebuffed());
            CurrentMob->GetBattleHandler()->GetBattleInfo()->ClearInfo();
        }

        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Immediate decision actions
    if (Decision != EDecision::None)
    {
        // Process decision and update Blackboard accordingly
        ProcessDecision(Decision, CurrentMob, BlackBoard, OwnerComp ,Target);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Decision is None: attack / start minigame
    StartAttackMinigame(CurrentMob, Target, Controller);
}

void UUBTTask_DefaultAtk::OnThinkComplete_Internal()
{
    AICC_AIController* Controller = Cast<AICC_AIController>(CurrentMob->GetController());
    if (!Controller || !BlackBoard) return;

    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(Controller->BrainComponent);
    if (!OwnerComp) return;

    OnThinkComplete(OwnerComp, Controller);
}

// Helper function: start attack minigame
void UUBTTask_DefaultAtk::StartAttackMinigame(AMob* Current, AICC_Player* Target, AICC_AIController* Controller)
{
    if (!Current || !Target || !Controller) return;

    Controller->MoveToActor(Target);

    Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
    DebugHelper::AddMessageToLog(Current->GetActorLabel() + " is attacking");

    Current->GetAIMemory().AttackLocation = Current->GetActorLocation();
	Target->GetBattleHUD()->DecisionDisplayer->Hide();
    Target->GetMinigameHandler()->StartMinigame(false);
	Target->GetMinigameHandler()->OnMinigameEnded.AddDynamic(this, &UUBTTask_DefaultAtk::OnMinigameEndedCallback);
    bBusy = true;
}

// Helper function: process decision (buff, heal, debuff, etc.)
void UUBTTask_DefaultAtk::ProcessDecision(EDecision Dec, AMob* Current, UBlackboardComponent* Board, UBehaviorTreeComponent* OwnerComp, AICC_Player* Target)
{
    AICC_AIController* Controller = Cast<AICC_AIController>(Current->GetController());
    if (!Controller || !Current || !BlackBoard || !OwnerComp || !Target)
        return;

    // Early-outs for ashamed or not-current-turn
    if (Current->IsAshamed())
    {
        Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " skipped the turn (Ashamed)"));
        DebugHelper::AddMessageToLog(Current->GetActorLabel() + " skipped the turn (Ashamed)");
        Current->SetTreeId(-1);
        Current->SetIsBuffedAtk(false);
        BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
        BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
        BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
        bBusy = false;
        Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    if (Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
    {
        DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not the " + Current->GetActorLabel() + " turn yet");
        DebugHelper::AddMessageToLog("It's not the " + Current->GetActorLabel() + " turn yet");
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Decisions
    switch (Dec)
    {
        case EDecision::BuffItSelf:
            if (Current->IsEAnger())
            {
                Current->SetIsBuffedAtk(true);
                BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
            }
            break;

        case EDecision::HealItSelf:
            if (Current->IsHealer())
            {
            	Current->SetHeal(true);
            	BlackBoard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
            }
            break;

        case EDecision::HealOther:
            if (Current->IsHealer())
            {
            	Current->SetHealOther(true);
            	BlackBoard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
            }
            break;

        case EDecision::DebuffDefence:
            if (Current->IsESadness() || Current->IsEAnxiety())
            {
                Current->GetBattleHandler()->GetBattleInfo()->SetTurnInfo(FText::FromString(Current->GetActorLabel() + " de-buff"));
                Current->SetIsTargetDefDebuffed(true);
                BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
            }
            break;

        case EDecision::BuffDefence:
            if (Current->IsEFear() || Current->IsECalm())
            {
                Current->SetBuffedDefence(true);
                BlackBoard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
            }
            break;

        case EDecision::BuffOtherDefence:
            if (Current->IsEFear() || Current->IsECalm())
            {
                Current->SetBuffOtherDefence(true);
                BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
            }
            break;

        case EDecision::DebuffAtk:
            if (Current->IsEDisgust() || Current->IsEAnxiety())
            {
                Current->SetPlayerDebuffAttack(true);
                BlackBoard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
            }
            break;

        case EDecision::FreezedUp:
            if (Current->IsEAnxiety())
            {
                Current->SetIsFreezedUp(true);
                BlackBoard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
            }
            break;

        case EDecision::BuffOther:
            if (Current->IsEJealousy())
            {
                Current->SetBuffOtherAtk(true);
                BlackBoard->SetValueAsBool("IsBuffOtherAtk?", Current->GetBuffOtherAtk());
            }
            break;

        case EDecision::EnvyBurned:
            if (Current->IsEJealousy())
            {
                Current->SetIsEnvyBurned(true);
                BlackBoard->SetValueAsBool("IsEnvyBurnedState?", Current->GetIsIsEnvyBurned());
            }
            break;

        case EDecision::DebuffShieldItSelf:
            if (Current->IsECalm())
            {
                Current->SetDebuffShield(true);
                BlackBoard->SetValueAsBool("IsShieldDebuffed?", Current->GetIsDebuffShield());
            }
            break;

        case EDecision::DebuffShieldOther:
            if (Current->IsECalm())
            {
                Current->SetDebuffOtherShield(true);
                BlackBoard->SetValueAsBool("IsOtherShieldDebuffed?", Current->GetIsDebuffOtherShield());
            }
            break;

        case EDecision::None:
        default:
            // Attack / start minigame
            StartAttackMinigame(Current, Target, Controller);
            return;
    }

    // Finish task after processing decision
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UUBTTask_DefaultAtk::OnMinigameEndedCallback()
{
	if (!CurrentMob || !BlackBoard) return;

	// Unsubscribe so it doesn't trigger multiple times
	if (AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target")))
	{
		Target->GetMinigameHandler()->OnMinigameEnded.RemoveDynamic(this, &UUBTTask_DefaultAtk::OnMinigameEndedCallback);
	}

	// Reset Blackboard / AI state
	CurrentMob->SetTreeId(-1);
	CurrentMob->SetIsBuffedAtk(false);
	BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
	BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());

	// Finish the task
	FinishLatentTask(*TreeComp, EBTNodeResult::Succeeded);
}


// BlackBoard = OwnerComp.GetBlackboardComponent();
//
// AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
// AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
// AMob* Self = Cast<AMob>(BlackBoard->GetValueAsObject("SelfActor"));
// checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));
//
// checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask %s "), *Self->GetData()->GetName());
//
//
// bool Attacked = BlackBoard->GetValueAsBool("Attacked?");
// AMob* Current = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
// checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));
//
// Current->SetTreeId(0);
// Current->SetIsAttacked(false);
// BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
// BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
//
// DecisionMaker.Setup(Current);
// Decision = DecisionMaker.Thought();
// Target->GetBattleHUD()->DecisionDisplayer->SetEnganger(FText::FromString(Current->GetActorLabel() + " is thinking..."));
//
// return EBTNodeResult::InProgress;



// AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	//
	// AMob* Current = Cast<AMob>(Controller->GetPawn());
	//
	// if (Current->IsAshamed())
	// {
	// 	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " skipped the turn (Ashamed)"));
	// 	DebugHelper::AddMessageToLog(Current->GetActorLabel() + " skipped the turn (Ashamed)");
	// 	Current->SetTreeId(-1);
	// 	Current->SetIsBuffedAtk(false);
	// 	BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	// 	BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	// 	BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
	// 	bBusy = false;
	// 	Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// 	return;
	// }
	//
	// if (Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
	// {
	// 	DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not the " + Current->GetActorLabel() + " turn yet");
	// 	DebugHelper::AddMessageToLog("It's not the " + Current->GetActorLabel() + " turn yet");
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// // -------   REWRITE THIS PART -----------------
	//
	// if (Decision == EDecision::BuffItSelf && Current->IsEAnger())
	// {
	// 	Current->SetIsBuffedAtk(true);
	// 	BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	// 	Decision = EDecision::Invalid;
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::HealItSelf && Current->IsHealer())
	// {
	// 	Current->SetHeal(true);
	// 	BlackBoard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::HealOther && Current->IsHealer())
	// {
	// 	Current->SetHealOther(true);
	// 	BlackBoard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if ((Decision == EDecision::DebuffDefence && Current->IsESadness()) || (Decision == EDecision::DebuffDefence && Current->IsEAnxiety()))
	// {
	// 	Current->GetBattleHandler()->GetBattleInfo()->SetTurnInfo(FText::FromString(Current->GetActorLabel() + " de-buff"));
	// 	Current->SetIsTargetDefDebuffed(true);
	// 	BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if ((Decision == EDecision::BuffDefence && Current->IsEFear()) || (Decision == EDecision::BuffDefence && Current->IsECalm()))
	// {
	// 	Current->SetBuffedDefence(true);
	// 	BlackBoard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
	// 	Decision = EDecision::Invalid;
	// 	DecisionMaker.Clear();
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if ((Decision == EDecision::BuffOtherDefence && Current->IsEFear()) || (Decision == EDecision::BuffOtherDefence && Current->IsECalm()))
	// {
	// 	Current->SetBuffOtherDefence(true);
	// 	BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
	// 	Decision = EDecision::Invalid;
	// 	DecisionMaker.Clear();
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if ((Decision == EDecision::DebuffAtk && Current->IsEDisgust()) || (Decision == EDecision::DebuffAtk && Current->IsEAnxiety()))
	// {
	// 	Current->SetPlayerDebuffAttack(true);
	// 	BlackBoard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::FreezedUp && Current->IsEAnxiety())
	// {
	// 	Current->SetIsFreezedUp(true);
	// 	BlackBoard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::BuffOther && Current->IsEJealousy())
	// {
	// 	Current->SetBuffOtherAtk(true);
	// 	BlackBoard->SetValueAsBool("IsBuffOtherAtk??", Current->GetBuffOtherAtk());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::EnvyBurned && Current->IsEJealousy())
	// {
	// 	Current->SetIsEnvyBurned(true);
	// 	BlackBoard->SetValueAsBool("IsEnvyBurnedState?", Current->GetIsIsEnvyBurned());
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::DebuffShieldItSelf && Current->IsECalm())
	// {
	// 	Current->SetDebuffShield(true);
	// 	BlackBoard->SetValueAsBool("IsShieldDebuffed??", Current->GetIsDebuffShield());
	// 	Decision = EDecision::Invalid;
	// 	DecisionMaker.Clear();
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	// else if (Decision == EDecision::DebuffShieldOther && Current->IsECalm())
	// {
	// 	Current->SetDebuffOtherShield(true);
	// 	BlackBoard->SetValueAsBool("IsOtherShieldDebuffed??", Current->GetIsDebuffOtherShield());
	// 	Decision = EDecision::Invalid;
	// 	DecisionMaker.Clear();
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// }
	//
	//
	// else if (Decision == EDecision::None)
	// {
	// 	Controller->MoveToActor(Target);
	// 	
	// 	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
	// 	DebugHelper::AddMessageToLog(Current->GetActorLabel() + " is attacking");
	//
	// 	constexpr float BaseThreshold = 95.f;
	// 	const float Distance = FVector::Dist(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation());
	// 	float DynamicThreshold = BaseThreshold * (Distance / 95.f); // Scale threshold based on distance factor
	// 	
	// 	if (FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < DynamicThreshold && !bBusy) //(FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < 95.f && !bBusy) 
	// 	{
	// 		Cast<AMob>(Controller->GetPawn())->GetAIMemory().AttackLocation = Cast<AMob>(Controller->GetPawn())->
	// 			GetActorLocation();
	// 		Target->GetMinigameHandler()->StartMinigame(false);
	// 		bBusy = true;
	// 	}
	//
	// 	// -------   REWRITE THIS PART -----------------
	//
	// 	
	// 	if (!Current->IsMinigameStarted() && Current->IsMinigameEnded() && Target->GetMinigameHandler()->IsPlayerMinigameEnded()) //if (!AMob::IsMinigameStarted() && AMob::MinigameEnded && Target->GetMinigameHandler()->IsPlayerMinigameEnded())
	// 	{
	// 		Current->SetTreeId(-1);
	// 		Current->SetIsBuffedAtk(false);
	// 		BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	// 		BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	// 		BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
	// 		bBusy = false;
	// 		Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
	// 		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// 	}
	// }