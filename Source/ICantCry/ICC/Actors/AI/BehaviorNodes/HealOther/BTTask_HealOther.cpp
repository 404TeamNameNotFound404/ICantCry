// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_HealOther.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTTask_HealOther::UBTTask_HealOther()
{
	NodeName = "HealOther";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_HealOther::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_DebuffOtherShield::ExecuteTask"))
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current invalid atUBTTask_DebuffOtherShield::ExecuteTask"))

	Current->SetHealOther(true);
	Current->SetIsAttacked(false);
	
	Blackboard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
	Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	AICC_Player* Target = Cast<AICC_Player>(Blackboard->GetValueAsObject("Target"));
	checkf(Target, TEXT("Player invalid at UBTTask_DebuffOtherShield::ExecuteTask"))


	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().CantBuffOthers())
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " attempted to heal other but it's alone! , rethink the action");
		Blackboard->SetValueAsBool("Rethinker", true);
		return EBTNodeResult::Succeeded;
	}
	
	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue(Current);
	if (!TargetToBuff) // rethink if targetto buff appears to be nullptr again 
	{
		Blackboard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " attempted to buff other def but it didn't find a valid target (scrivetemelo se entra qua dentro)! , rethinking the action");
		return EBTNodeResult::Succeeded;
	}

	//TODO  Debuff TargetToBuff shield
	
	AMob* CurrentToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue();
	checkf(CurrentToBuff, TEXT("Invalid"));
	

	// if (CurrentToBuff->GetData()->Health < CurrentToBuff->GetData()->MaxHealth)
	// {
	// 	CurrentToBuff->Heal(Current->GetData()->Health *= 0.20f);
	// }

	if (CurrentToBuff->GetStats().Health < CurrentToBuff->GetData()->MaxHealth)
	{
		CurrentToBuff->Heal(CurrentToBuff->GetStats().Health *= 0.20f);
	}
	
	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " heals " + CurrentToBuff->GetActorLabel()));
	Target->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(""));
	DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " heals " + CurrentToBuff->GetActorLabel());
	
	return EBTNodeResult::InProgress;
}

void UBTTask_HealOther::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTimeStarted)
		return;
	
	bTimeStarted = true;

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());

	if (!Current || !Controller)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Current != Current->GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("Debuff Other Shield Task Completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: Debuff Other Shield Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_HealOther::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetHealOther(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (Blackboard)
		{
			Blackboard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
			Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		}

		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		
	}

	bTimeStarted = false;
}
