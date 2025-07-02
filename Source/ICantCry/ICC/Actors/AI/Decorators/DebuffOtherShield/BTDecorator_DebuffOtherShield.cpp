// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_DebuffOtherShield.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_DebuffOtherShield::UBTDecorator_DebuffOtherShield()
{
	NodeName = "DebuffOtherShield";
}

bool UBTDecorator_DebuffOtherShield::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsOtherShieldDebuffed?");
	return Buff;
}
