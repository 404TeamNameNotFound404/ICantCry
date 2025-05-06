// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_BuffOtherAtk.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_BuffOtherAtk::UBTDecorator_BuffOtherAtk()
{
	NodeName = "BuffOtherAtk";
}

bool UBTDecorator_BuffOtherAtk::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsBuffOtherAtk?");
	return Buff;
}
