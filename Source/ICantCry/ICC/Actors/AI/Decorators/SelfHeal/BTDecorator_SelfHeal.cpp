// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_SelfHeal.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_SelfHeal::UBTDecorator_SelfHeal()
{
	NodeName = "SelfHeal";
}

bool UBTDecorator_SelfHeal::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool bHeal = Blackboard->GetValueAsBool("IsHealing?");
	return bHeal;
}
