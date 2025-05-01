// Fill out your copyright notice in the Description page of Project Settings.
#include "BTDecorator_IdCondition.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IdCondition::UBTDecorator_IdCondition()
{
	NodeName = FString("Id Evaluation");
	IdKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IdCondition, IdKey));
}

bool UBTDecorator_IdCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	const int32 CurrentId = BB->GetValueAsInt(IdKey.SelectedKeyName);
	
	return CurrentId == Evaluation;
}
