// Fill out your copyright notice in the Description page of Project Settings.
#include "BTDecorator_CheckAITurn.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

UBTDecorator_CheckAITurn::UBTDecorator_CheckAITurn()
{
	NodeName = "AI Turn";
}

bool UBTDecorator_CheckAITurn::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	checkf(BB, TEXT("Blackboard component is invalid inUBTDecorator_CheckAITurn"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Invalid AIOwner"));
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Invalid AMob"));

	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetIsAITurn())
	{
		return true;
	}

	return false;
}
