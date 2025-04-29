// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_LowHealth.generated.h"

/**
 * This decorator checks if AI health is low (20% health left)
 */
UCLASS()
class ICANTCRY_API UBTDecorator_LowHealth : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_LowHealth();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
