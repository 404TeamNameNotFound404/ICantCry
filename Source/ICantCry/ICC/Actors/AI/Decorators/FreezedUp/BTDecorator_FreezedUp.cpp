// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_FreezedUp.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_FreezedUp::UBTDecorator_FreezedUp()
{
	NodeName = "IsFreezedUp?";
}

bool UBTDecorator_FreezedUp::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsFreezedUp?");
	return Buff;
}
