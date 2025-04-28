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
}

EBTNodeResult::Type UUUBTTask_Retreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	BlackBoard->SetValueAsInt("Id", -1);
	BlackBoard->SetValueAsBool("Attacked?", true);

	Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUUBTTask_Retreat::ExecuteTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at EBTNodeResult::Type AICC_AIController::GetPawn"));
	
	FAIMoveRequest Request;
	Request.SetGoalLocation(Current->GetAIMemory().DefaultBattleLocation);
	Controller->MoveTo(Request);
	
	
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
		const float Distance = FVector::Distance(Current->GetAIMemory().DefaultBattleLocation, Current->GetActorLocation());
		
		if (Distance < 60.f) // 40
		{
			bRetreated = true;
		}
	}

	if (bRetreated && !bTimerStarted)
	{
		bTimerStarted = true; // Prevent starting multiple timers
        
		Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, &OwnerComp, Controller]()
		{
			BlackBoard->SetValueAsInt("Id", 0);
			BlackBoard->SetValueAsBool("Attacked?", false); // Now attack mode
			AMob::MinigameEnded = false;
			AMob::SetMinigameStarted(false);

			Controller->BrainComponent->StopLogic("End Turn");

			Target->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
			Target->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
			Target->GetBattleHUD()->GetCurrentPlayingEmotion()->SetIsBusy(false);

			bRetreated = false;
			bTimerStarted = false;

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		}, 0.5f, false); // Half-second delay
	}
}
