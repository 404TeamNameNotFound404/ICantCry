// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FreezedUp.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTTask_FreezedUp::UBTTask_FreezedUp()
{
	NodeName = "FreezedUp";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_FreezedUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_FreezedUp::ExecuteTask"))
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current invalid at UBTTask_FreezedUp::ExecuteTask"))

	Current->SetIsFreezedUp(true);
	Current->SetIsAttacked(false);

	
	Blackboard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
	Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	AICC_Player* Target = Cast<AICC_Player>(Blackboard->GetValueAsObject("Target"));
	checkf(Target, TEXT("Player invalid at UBTTask_FreezedUp::ExecuteTask"))
	
	Target->GetStatusTracker()->InflictStatus(EAfflictedStatus::Freezed, Target);

	//TODO Implement Player->Freeze();

	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " freezed " + Target->GetActorLabel()));
	DebugHelper::AddMessageToLog(Current->GetActorLabel() + " freezed " + Target->GetActorLabel());

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " freezed " + Target->GetActorLabel()));
	
	return EBTNodeResult::InProgress;
}

void UBTTask_FreezedUp::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		DebugHelper::AddMessageToLog("It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("Debuff Defence Task Completed");
		DebugHelper::AddMessageToLog("Debuff Defence Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_FreezedUp::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetIsFreezedUp(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (Blackboard)
		{
			Blackboard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
			Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		}

		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}

	bTimeStarted = false;
}
