// Fill out your copyright notice in the Description page of Project Settings.
#include "UUBTTask_Retreat.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Navigation/PathFollowingComponent.h"

UUUBTTask_Retreat::UUUBTTask_Retreat()
{
	NodeName = TEXT("Retreat");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
	bRetreated = false;
	bTimerStarted = false;
}

EBTNodeResult::Type UUUBTTask_Retreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUUBTTask_Retreat::ExecuteTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at EBTNodeResult::Type AICC_AIController::GetPawn"));

	Current->SetTreeId(-1);
	Current->SetIsAttacked(true);
	
	BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	
	FAIMoveRequest Request;
	Request.SetGoalLocation(Current->GetAIMemory().DefaultBattleLocation);
	Controller->MoveTo(Request);
	bRetreated = false;
	bTimerStarted = false;
	
	return EBTNodeResult::InProgress;
}

void UUUBTTask_Retreat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at EBTNodeResult::TickTask"));

	if (!bRetreated)
	{
		const FVector& DefaultLocation = Current->GetAIMemory().DefaultBattleLocation;
		float ActualDistance = FVector::Dist(DefaultLocation, Current->GetAIMemory().AttackLocation);
		const float Distance = FVector::Distance(Current->GetAIMemory().DefaultBattleLocation, Current->GetActorLocation());
		
		if (const float Threshold = Distance / ActualDistance <= 0.8f)  // 60 - 40
		{
			DebugHelper::LogError("Returned in position");
			bRetreated = true;
		}
	}

	if (bRetreated && !bTimerStarted)
	{
		bTimerStarted = true; // Prevent starting multiple timers
        
		Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, &OwnerComp, Controller, Current]()
		{
			
			Current->SetTreeId(0);
			Current->SetIsAttacked(false);
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
			BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked()); // Now attack mode
			AMob::MinigameEnded = false;
			AMob::SetMinigameStarted(false);

			Controller->BrainComponent->StopLogic("End Turn");

			// Target->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
			// Target->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
			Target->GetBattleHUD()->GetCurrentPlayingEmotion()->SetIsBusy(false);

			bRetreated = false;
			bTimerStarted = false;

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		}, 0.5f, false); // Half-second delay
	}
}

void UUUBTTask_Retreat::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at EBTNodeResult::TickTask"));

	Current->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
	Current->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
	Current->GetStatusTracker()->UpdateStatus();
	Current->GetStatusTracker()->UpdateBuffStatus();
}
