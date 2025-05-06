// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_HealOther.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HealOther::UBTDecorator_HealOther()
{
	NodeName = "Heal Other";
}

bool UBTDecorator_HealOther::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool bHealOther = Blackboard->GetValueAsBool("IsHealingOther?");
	return bHealOther;
}
