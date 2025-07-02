// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_BuffDefence.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_BuffDefence::UBTDecorator_BuffDefence()
{
	NodeName = "BuffDefence";
}

bool UBTDecorator_BuffDefence::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsDefenceBuffed?");
	return Buff;
}
