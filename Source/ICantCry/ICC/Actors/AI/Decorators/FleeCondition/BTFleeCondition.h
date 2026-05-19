// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTFleeCondition.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBTFleeCondition : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTFleeCondition();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
