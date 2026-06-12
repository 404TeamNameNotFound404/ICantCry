// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Flee.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

UBTTask_Flee::UBTTask_Flee()
{
	NodeName = "Flee";
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}


EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_EnvyBurned::ExecuteTask"))
	Current = Cast<AMob>(Controller->GetPawn());

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	OwnerRef = &OwnerComp;

	UTurnBasedSystem* TBS = Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem();
	
	if (const bool bCanFlee = TBS->GetTurn().LastMobStanding() || TBS->GetTurn().AreAnxietyOrCalmAlone(); !bCanFlee)
	{
		if (Current->GetMobType() == EMobType::MobAnxiety)
		{
			Current->SetPlayerDebuffAttack(true);
			Blackboard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
			DebugHelper::AddMessageToLog("[BT - Flee]: Anxiety is not the last emotion standing so forcing debuff atk");
			return EBTNodeResult::Succeeded;
		}

		else if (Current->GetMobType() == EMobType::MobCalm)
		{
			Current->SetBuffedDefence(true);
			Blackboard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
			DebugHelper::AddMessageToLog("[BT - Flee]: Calm is not the last emotion standing so forcing buff def");
			return EBTNodeResult::Succeeded;
		}
	}
	

	Attempts++;
	int32 SuccessRate = 0;

	DebugHelper::AddMessageToLog("[BT - Flee]: Attemps N: " + FString::FromInt(Attempts));
	DebugHelper::LogMessage(10, FColor::White, "Attemps: " + FString::FromInt(Attempts));


	if (Attempts == 1) SuccessRate = 33;
	else if (Attempts == 2) SuccessRate = 66;
	else SuccessRate = 100;

	if (const int32 DiceRoll = FMath::RandRange(1, 100);
		DiceRoll <= SuccessRate)
	{
		DebugHelper::AddMessageToLog("[BT - Flee]: Dice Roll N: " + FString::FromInt(DiceRoll));
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
			FText::FromString(Current->GetData()->EnemyName.ToString() + " ran away"));
		DebugHelper::AddMessageToLog(
			"[Behavior Tree - Flee]: " + Current->GetData()->EnemyName.ToString() + " ran away");

		FTimerHandle Delay;
		GetWorld()->GetTimerManager().SetTimer(
			Delay,
			this,
			&UBTTask_Flee::Run,
			1.5f,
			false);
	}
	else
	{
		DebugHelper::AddMessageToLog(
			"[Default BT]: " + Current->GetData()->EnemyName.ToString() + " tried to escape but it failed, chance was "
			+ FString::FromInt(DiceRoll));

		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
			FText::FromString(Current->GetData()->EnemyName.ToString() + " tried to escape but it failed"));

		FTimerHandle Delay;
		GetWorld()->GetTimerManager().SetTimer(
			Delay,
			this,
			&UBTTask_Flee::FailFlee,
			1.85f,
			false);
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_Flee::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	Current->SetFleeing(false);
	Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());

	FTimerHandle Delay;
	GetWorld()->GetTimerManager().SetTimer(Delay, [this]
	{
		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}, 1.25f, false);
}

void UBTTask_Flee::Run()
{
	const UICantCryGameInstance* Instance =
		Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()
	        ->GetBattleHUD()
	        ->DecisionDisplayer
	        ->Hide();
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->Flow();
	Current->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
	Current->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
	Current->SetFleeing(false);
	Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());
	FinishLatentTask(*OwnerRef, EBTNodeResult::Succeeded);
}

void UBTTask_Flee::FailFlee()
{
	if (const UICantCryGameInstance* Instance =
		Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (Instance->GetCurrentPlayer())
		{
			Instance->GetCurrentPlayer()
			        ->GetBattleHUD()
			        ->DecisionDisplayer
			        ->Hide();
		}
	}

	if (OwnerRef)
	{
		Current->SetFleeing(false);
		Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());
		Current->SetTreeId(-1);
		Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		FinishLatentTask(*OwnerRef, EBTNodeResult::Succeeded);
	}
}
