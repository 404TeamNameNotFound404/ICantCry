// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_Buff.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"

UBTDecorator_Buff::UBTDecorator_Buff()
{
	NodeName = "IsBuffed";
}

bool UBTDecorator_Buff::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsBuffed?");
	return Buff;
}
