// Fill out your copyright notice in the Description page of Project Settings.
#include "BTTask_Buff.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTTask_Buff::UBTTask_Buff()
{
	NodeName = "Buff";
	BlackBoard = nullptr;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Buff::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
// buff happen but weirdly enough join the retreat task
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetIsBuffedAtk(true);
	Current->SetIsAttacked(false);
	
	BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());



	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(
		FText::FromString(Current->GetActorLabel() + " is buffing atk"));

	return EBTNodeResult::InProgress;
}

void UBTTask_Buff::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bTimerStarted)
	{
		bTimerStarted = true;
		AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
		AMob* Current = Cast<AMob>(Controller->GetPawn());
		BlackBoard = OwnerComp.GetBlackboardComponent();

		Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, &OwnerComp, Current]()
		{
			if (UBrainComponent* Brain = OwnerComp.GetAIOwner()->BrainComponent)
			{
				Brain->StopLogic("Buff Finished");
			}

			Current->SetIsBuffedAtk(false);
			Current->SetTreeId(0);
			Current->SetIsAttacked(false);
			BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
			BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());

			Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
			Current->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
			Current->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
			
			// AMob* Mob = Cast<AMob>(OwnerComp.GetAIOwner()->GetPawn());
			// Mob->GetBattleHandler()->GetBattleInfo()->ClearInfo();
			// Mob->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
			// Mob->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();

			DebugHelper::LogSuccess("Buff Task Completed");
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		}), 1.0f, false);
	}
}

void UBTTask_Buff::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	// if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	// {
	// 	Controller->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	// }
	
	bTimerStarted = false;
}


