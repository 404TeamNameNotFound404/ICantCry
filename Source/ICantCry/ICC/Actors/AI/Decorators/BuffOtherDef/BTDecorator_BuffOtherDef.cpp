// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_BuffOtherDef.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_BuffOtherDef::UBTDecorator_BuffOtherDef()
{
	NodeName = "BuffOtherDef";
}

bool UBTDecorator_BuffOtherDef::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsBuffedOtherDef?");
	return Buff;
}
