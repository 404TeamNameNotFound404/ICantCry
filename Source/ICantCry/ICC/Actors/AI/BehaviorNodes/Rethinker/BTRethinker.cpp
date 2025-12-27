// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRethinker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTRethinker::UBTRethinker()
{
	NodeName = TEXT("Rethinker");
	bCreateNodeInstance = true;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTRethinker::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	BlackBoard = OwnerComp.GetBlackboardComponent();
	TreeComp = &OwnerComp;
	
	const AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	Current = Cast<AMob>(Controller->GetPawn());
	
	DebugHelper::AddMessageToLog("[Behavior Tree - Rethinker]: " + Current->GetData()->EnemyName.ToString() + " can't buff other .. rethinking a new action");
	const AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));

    Current->SetRethink(true);
    BlackBoard->SetValueAsBool("Rethinker", Current->GetRethink());
    
	bWaitingForThinkCompletion = true;
	bBusy = true;

	DecisionMaker.Clear();
	DecisionMaker.Setup(Current);
	Decision = DecisionMaker.Thought();

	Target->GetBattleHUD()->DecisionDisplayer->Show();
	Target->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
		FText::FromString(Current->GetData()->EnemyName.ToString() + " is re-thinking...")
	);
	
	Controller->GetWorld()->GetTimerManager().ClearTimer(DelayHandle);

	if (Delay > 0.f)
	{
		Controller->GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&UBTRethinker::OnThinkComplete_Internal,
			Delay,
			false
		);
	}
	else
	{
		OnThinkComplete_Internal();
	}
	
	return EBTNodeResult::InProgress;
}

// probably I won't need this?

void UBTRethinker::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bWaitingForThinkCompletion || !bBusy)
		return;
	
	if (const AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()); !Controller || !Current || !BlackBoard)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!Current->IsMinigameStarted() || !Current->IsMinigameEnded() || !Target->GetMinigameHandler()->IsPlayerMinigameEnded())
		return;
	
	// Current->SetTreeId(-1);
	// Current->SetIsBuffedAtk(false);
	// BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	// BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	// BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
	// Current->SetRethink(false);
	// BlackBoard->SetValueAsBool("Rethinker", Current->GetRethink());
	//
	// Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
	//
	// bBusy = false;
	// bWaitingForThinkCompletion = false;
	//
	// FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTRethinker::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTRethinker::OnThinkComplete(UBehaviorTreeComponent* OwnerComp, AICC_AIController* Controller)
{
	if (!OwnerComp || !Controller || !Current || !BlackBoard)
		return;

	bWaitingForThinkCompletion = true;

	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	if (!Target)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (Current->IsAshamed() || Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		if (Current->IsAshamed())
		{
			Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetData()->EnemyName.ToString() + " skipped the turn (Ashamed)"));
			DebugHelper::AddMessageToLog("[Behavior Tree - Rethinker]: " + Current->GetData()->EnemyName.ToString() + " skipped the turn (Ashamed)");

			Current->SetTreeId(-1);
			Current->SetIsBuffedAtk(false);
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
			BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
			BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
			Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		}

		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	if (Decision != EDecision::None)
	{
		ProcessDecision(Decision, Current, BlackBoard, OwnerComp ,Target);
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	StartAttackMinigame(Current, Target, Controller);
}

void UBTRethinker::OnThinkComplete_Internal()
{
    AICC_AIController* Controller = Cast<AICC_AIController>(Current->GetController());
    if (!Controller || !BlackBoard) return;

    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(Controller->BrainComponent);
    if (!OwnerComp) return;

    OnThinkComplete(OwnerComp, Controller);
}

void UBTRethinker::StartAttackMinigame(AMob* CurrentMob, AICC_Player* Target, AICC_AIController* Controller)
{
    if (!CurrentMob || !Target || !Controller) return;

    Controller->MoveToActor(Target);

    CurrentMob->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
    DebugHelper::AddMessageToLog("[Behavior Tree - Rethinker]: " + Current->GetActorLabel() + " is attacking");

    CurrentMob->GetAIMemory().AttackLocation = CurrentMob->GetActorLocation();
	Target->GetBattleHUD()->DecisionDisplayer->Hide();
    Target->GetMinigameHandler()->StartMinigame(false);
	Target->GetMinigameHandler()->OnMinigameEnded.AddDynamic(this, &UBTRethinker::OnMinigameEndedCallback);
    bBusy = true;
}

void UBTRethinker::ProcessDecision(EDecision Dec, AMob* CurrentMob, UBlackboardComponent* Board, UBehaviorTreeComponent* OwnerComp, AICC_Player* Target)
{
    AICC_AIController* Controller = Cast<AICC_AIController>(Current->GetController());
    if (!Controller || !Current || !BlackBoard || !OwnerComp || !Target)
        return;
	
    if (Current->IsAshamed())
    {
        CurrentMob->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " skipped the turn (Ashamed)"));
        DebugHelper::AddMessageToLog("[Behavior Tree - Rethinker]: " + CurrentMob->GetActorLabel() + " skipped the turn (Ashamed)");
        CurrentMob->SetTreeId(-1);
        CurrentMob->SetIsBuffedAtk(false);
        BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
        BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
        BlackBoard->SetValueAsBool("IsDefenceDebuffed?", CurrentMob->GetIsTargetDefenceDebuffed());
        bBusy = false;
        CurrentMob->GetBattleHandler()->GetBattleInfo()->ClearInfo();
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    if (Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
    {
        DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not the " + Current->GetActorLabel() + " turn yet");
        DebugHelper::AddMessageToLog("[Behavior Tree - Rethinker]: It's not the " + Current->GetActorLabel() + " turn yet");
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
	
    switch (Dec)
    {
        case EDecision::BuffItSelf:
            if (Current->IsEAnger())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetIsBuffedAtk(true);
                BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
            }
            break;

        case EDecision::HealItSelf:
            if (CurrentMob->IsHealer())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
            	CurrentMob->SetHeal(true);
            	BlackBoard->SetValueAsBool("IsHealing?", CurrentMob->GetIsHeal());
            }
            break;

        case EDecision::HealOther:
            if (CurrentMob->IsHealer())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
            	CurrentMob->SetHealOther(true);
            	BlackBoard->SetValueAsBool("IsHealingOther?", CurrentMob->GetIsHealOther());
            }
            break;

        case EDecision::DebuffDefence:
            if (CurrentMob->IsESadness() || CurrentMob->IsEAnxiety())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->GetBattleHandler()->GetBattleInfo()->SetTurnInfo(FText::FromString(Current->GetActorLabel() + " de-buff"));
                CurrentMob->SetIsTargetDefDebuffed(true);
                BlackBoard->SetValueAsBool("IsDefenceDebuffed?", CurrentMob->GetIsTargetDefenceDebuffed());
            }
            break;

        case EDecision::BuffDefence:
            if (CurrentMob->IsEFear() || CurrentMob->IsECalm())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetBuffedDefence(true);
                BlackBoard->SetValueAsBool("IsDefenceBuffed?", CurrentMob->GetIsBuffedDefence());
            }
            break;

        case EDecision::BuffOtherDefence:
            if (CurrentMob->IsEFear() || CurrentMob->IsECalm())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetBuffOtherDefence(true);
                BlackBoard->SetValueAsBool("IsBuffedOtherDef?", CurrentMob->GetBuffOtherDefence());
            }
            break;

        case EDecision::DebuffAtk:
            if (CurrentMob->IsEDisgust() || CurrentMob->IsEAnxiety())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetPlayerDebuffAttack(true);
                BlackBoard->SetValueAsBool("IsAttackDebuffed?", CurrentMob->GetPlayerDebuffAttack());
            }
            break;

        case EDecision::FreezedUp:
            if (CurrentMob->IsEAnxiety())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetIsFreezedUp(true);
                BlackBoard->SetValueAsBool("IsFreezedUp?", CurrentMob->GetIsIsFreezedUp());
            }
            break;

        case EDecision::BuffOther:
            if (CurrentMob->IsEJealousy())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetBuffOtherAtk(true);
                BlackBoard->SetValueAsBool("IsBuffOtherAtk?", CurrentMob->GetBuffOtherAtk());
            }
            break;

        case EDecision::EnvyBurned:
            if (CurrentMob->IsEJealousy())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetIsEnvyBurned(true);
                BlackBoard->SetValueAsBool("IsEnvyBurnedState?", CurrentMob->GetIsIsEnvyBurned());
            }
            break;

        case EDecision::DebuffShieldItSelf:
            if (CurrentMob->IsECalm())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetDebuffShield(true);
                BlackBoard->SetValueAsBool("IsShieldDebuffed?", CurrentMob->GetIsDebuffShield());
            }
            break;

        case EDecision::DebuffShieldOther:
            if (CurrentMob->IsECalm())
            {
            	CurrentMob->SetRethink(false);
            	BlackBoard->SetValueAsBool("Rethinker", CurrentMob->GetRethink());
                CurrentMob->SetDebuffOtherShield(true);
                BlackBoard->SetValueAsBool("IsOtherShieldDebuffed?", CurrentMob->GetIsDebuffOtherShield());
            }
            break;

        case EDecision::None:
        default:
            StartAttackMinigame(CurrentMob, Target, Controller);
            return;
    }
	
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTRethinker::OnMinigameEndedCallback()
{
	if (!Current || !BlackBoard) return;
	
	if (const AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target")))
	{
		Target->GetMinigameHandler()->OnMinigameEnded.RemoveDynamic(this, &UBTRethinker::OnMinigameEndedCallback);
	}
	
	Current->SetTreeId(-1);
	Current->SetIsBuffedAtk(false);
	BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	Current->SetRethink(false);
	BlackBoard->SetValueAsBool("Rethinker", Current->GetRethink());
	
	FinishLatentTask(*TreeComp, EBTNodeResult::Succeeded);
}
