// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_DebuffAtk.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_DebuffAtk::UBTDecorator_DebuffAtk()
{
	NodeName = "Debuff Attack";
}

bool UBTDecorator_DebuffAtk::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool Buff = Blackboard->GetValueAsBool("IsAttackDebuffed?");
	return Buff;
}
