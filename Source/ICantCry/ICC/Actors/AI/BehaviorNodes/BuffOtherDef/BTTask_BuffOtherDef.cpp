// Fill out your copyright notice in the Description page of Project Settings.
#include "BTTask_BuffOtherDef.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


UBTTask_BuffOtherDef::UBTTask_BuffOtherDef()
{
	NodeName = "BuffOtherDef";
}

EBTNodeResult::Type UBTTask_BuffOtherDef::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetBuffOtherDefence(true);
	Current->SetIsAttacked(false);
	
	BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());

	//TODO ADD a counter for the buff (must last 3 turns)
	

	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue();
	checkf(TargetToBuff, TEXT("TargetToBuff is invalid Type UBTTask_BuffDefence::ExecuteTask"))

	TargetToBuff->GetData()->DefencePower *= 0.20f; // still assuming a 20% increase
	

	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(
		FText::FromString(Current->GetActorLabel() + " buffed " + TargetToBuff->GetActorLabel() + " def"));
	
	return EBTNodeResult::InProgress;
}

void UBTTask_BuffOtherDef::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (bTimerStarted)
		return;

	bTimerStarted = true;

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());

	if (!Current || !Controller)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Current != Current->GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not " + Current->GetActorLabel() + "'s turn yet (buff def task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("buff def target task Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_BuffOtherDef::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetBuffOtherDefence(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (BlackBoard)
		{
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", Current->GetBuffOtherDefence());
			BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
		}

		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
	}

	bTimerStarted = false;
}
