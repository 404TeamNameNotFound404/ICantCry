// Fill out your copyright notice in the Description page of Project Settings.


#include "BTFleeCondition.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTFleeCondition::UBTFleeCondition()
{
	NodeName = "Flee Condition";
}

bool UBTFleeCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Fleeing = Blackboard->GetValueAsBool("Flee?");
	return Fleeing;
}
