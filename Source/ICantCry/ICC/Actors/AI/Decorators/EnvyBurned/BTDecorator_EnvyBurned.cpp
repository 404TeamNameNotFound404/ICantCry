// Fill out your copyright notice in the Description page of Project Settings.
#include "BTDecorator_EnvyBurned.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_EnvyBurned::UBTDecorator_EnvyBurned()
{
	NodeName = "EnvyBurned";
}

bool UBTDecorator_EnvyBurned::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsEnvyBurnedState?");
	return Buff;
}
