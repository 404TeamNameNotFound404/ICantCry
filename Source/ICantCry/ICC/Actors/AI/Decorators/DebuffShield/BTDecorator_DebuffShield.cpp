// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_DebuffShield.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_DebuffShield::UBTDecorator_DebuffShield()
{
	NodeName = "Decorator_DebuffShield";
}

bool UBTDecorator_DebuffShield::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsShieldDebuffed?");
	return Buff;
}
