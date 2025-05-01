// Fill out your copyright notice in the Description page of Project Settings.
#include "BTDecorator_LowHealth.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

UBTDecorator_LowHealth::UBTDecorator_LowHealth()
{
	NodeName = "Low Health";
}

bool UBTDecorator_LowHealth::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Bb = OwnerComp.GetBlackboardComponent();

	AICC_AIController* AIController = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(AIController, TEXT("AIController is invalid at bool UBTDecorator_LowHealth::CalculateRawConditionValue"));

	AMob* Current = Cast<AMob>(AIController->GetPawn());
	checkf(Current, TEXT("Mob is invalid at decorator UBTDecorator_LowHealth::CalculateRawConditionValue"));
	
	
	if (!Current->IsHealer())
	{
		return false;
	}

	return Current->GetHealthBar()->IsLow();
}
