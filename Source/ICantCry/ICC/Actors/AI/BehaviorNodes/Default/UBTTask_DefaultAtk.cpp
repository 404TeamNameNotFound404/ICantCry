#include "UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UUBTTask_DefaultAtk::UUBTTask_DefaultAtk()
{
	NodeName = TEXT("NoStatusAttack");
	bNotifyTick = true;
}

EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Self = Cast<AMob>(BlackBoard->GetValueAsObject("SelfActor"));
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask %s "), *Self->GetData()->GetName());


	bool Attacked = BlackBoard->GetValueAsBool("Attacked?");
	AMob* Current = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetTreeId(0);
	Current->SetIsAttacked(false);
	BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());


	FDecisionMaker DecisionMaker;

	DecisionMaker.Clear(); // If I'm not wrong clearing before adding new states will avoid repetitions
	
	if (Current->IsEAnger())
	{
		DecisionMaker.DecisionMap.Add(EDecision::BuffItSelf, 0.55); // Buff atk chance in normal status
	}

	if (Current->IsHealer())
	{
		DecisionMaker.DecisionMap.Add(EDecision::HealItSelf, 0.10);
		DecisionMaker.DecisionMap.Add(EDecision::HealOther, 0.20);
	}

	if (Current->IsESadness())
	{
		DecisionMaker.DecisionMap.Add(EDecision::DebuffDefence, 0.60); // debuff target defence in normal status
	}

	if (Current->IsEDisgust())
	{
		DecisionMaker.DecisionMap.Add(EDecision::DebuffAtk, 0.60);
	}

	if (Current->IsEFear())
	{
		DecisionMaker.DecisionMap.Add(EDecision::BuffDefence, 0.60);
		DecisionMaker.DecisionMap.Add(EDecision::BuffOtherDefence, 0.20);
	}

	if (Current->IsEAnxiety())
	{
		DecisionMaker.DecisionMap.Add(EDecision::DebuffAtk, 0.30);
		DecisionMaker.DecisionMap.Add(EDecision::DebuffDefence, 0.30);
		DecisionMaker.DecisionMap.Add(EDecision::FreezedUp, 0.40);
	}

	if (Current->IsEJealousy())
	{
		DecisionMaker.DecisionMap.Add(EDecision::EnvyBurned, 0.50);
		DecisionMaker.DecisionMap.Add(EDecision::BuffOther, 0.20);
	}

	if (Current->IsECalm())
	{
		DecisionMaker.DecisionMap.Add(EDecision::DebuffShieldItSelf, 0.60);
		DecisionMaker.DecisionMap.Add(EDecision::DebuffShieldOther, 0.15);
		DecisionMaker.DecisionMap.Add(EDecision::BuffDefence, 0.10);
		DecisionMaker.DecisionMap.Add(EDecision::BuffOtherDefence, 0.20);
	}
	
	Decision = DecisionMaker.Thought();

	// Controller->MoveToActor(Target);
	//Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));

	return EBTNodeResult::InProgress;
}

void UUBTTask_DefaultAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());

	if (Current->IsAshamed())
	{
		Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " skipped the turn (Ashamed)"));
		Current->SetTreeId(-1);
		Current->SetIsBuffedAtk(false);
		BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
		BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
		BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
		bBusy = false;
		Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not the " + Current->GetActorLabel() + " turn yet");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	// -------   REWRITE THIS PART -----------------
	
	if (Decision == EDecision::BuffItSelf && Current->IsEAnger())
	{
		Current->SetIsBuffedAtk(true);
		BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::HealItSelf && Current->IsHealer())
	{
		Current->SetHeal(true);
		BlackBoard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::HealOther && Current->IsHealer())
	{
		Current->SetHealOther(true);
		BlackBoard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if ((Decision == EDecision::DebuffDefence && Current->IsESadness()) || (Decision == EDecision::DebuffDefence && Current->IsEAnxiety()))
	{
		Current->GetBattleHandler()->GetBattleInfo()->SetTurnInfo(FText::FromString(Current->GetActorLabel() + " de-buff"));
		Current->SetIsTargetDefDebuffed(true);
		BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if ((Decision == EDecision::BuffDefence && Current->IsEFear()) || (Decision == EDecision::BuffDefence && Current->IsECalm()))
	{
		Current->SetBuffedDefence(true);
		BlackBoard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if ((Decision == EDecision::BuffOtherDefence && Current->IsEFear()) || (Decision == EDecision::BuffOtherDefence && Current->IsECalm()))
	{
		Current->SetBuffOtherDefence(true);
		BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if ((Decision == EDecision::DebuffAtk && Current->IsEDisgust()) || (Decision == EDecision::DebuffAtk && Current->IsEAnxiety()))
	{
		Current->SetPlayerDebuffAttack(true);
		BlackBoard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::FreezedUp && Current->IsEAnxiety())
	{
		Current->SetIsFreezedUp(true);
		BlackBoard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::BuffOther && Current->IsEJealousy())
	{
		Current->SetBuffOtherAtk(true);
		BlackBoard->SetValueAsBool("IsBuffOtherAtk??", Current->GetBuffOtherAtk());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::EnvyBurned && Current->IsEJealousy())
	{
		Current->SetIsEnvyBurned(true);
		BlackBoard->SetValueAsBool("IsEnvyBurnedState?", Current->GetIsIsEnvyBurned());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::DebuffShieldItSelf && Current->IsECalm())
	{
		Current->SetDebuffShield(true);
		BlackBoard->SetValueAsBool("IsShieldDebuffed??", Current->GetIsDebuffShield());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (Decision == EDecision::DebuffShieldOther && Current->IsECalm())
	{
		Current->SetDebuffOtherShield(true);
		BlackBoard->SetValueAsBool("IsOtherShieldDebuffed??", Current->GetIsDebuffOtherShield());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	

	if (Decision == EDecision::None)
	{
		Controller->MoveToActor(Target);
		
		Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));

		constexpr float BaseThreshold = 95.f;
		const float Distance = FVector::Dist(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation());
		float DynamicThreshold = BaseThreshold * (Distance / 95.f); // Scale threshold based on distance factor
		
		if (FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < DynamicThreshold && !bBusy) //(FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < 95.f && !bBusy) 
		{
			Cast<AMob>(Controller->GetPawn())->GetAIMemory().AttackLocation = Cast<AMob>(Controller->GetPawn())->
				GetActorLocation();
			Target->GetMinigameHandler()->StartMinigame(false);
			bBusy = true;
		}

		// -------   REWRITE THIS PART -----------------

		
		if (!Current->IsMinigameStarted() && Current->IsMinigameEnded() && Target->GetMinigameHandler()->IsPlayerMinigameEnded()) //if (!AMob::IsMinigameStarted() && AMob::MinigameEnded && Target->GetMinigameHandler()->IsPlayerMinigameEnded())
		{
			Current->SetTreeId(-1);
			Current->SetIsBuffedAtk(false);
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
			BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
			BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
			bBusy = false;
			Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
