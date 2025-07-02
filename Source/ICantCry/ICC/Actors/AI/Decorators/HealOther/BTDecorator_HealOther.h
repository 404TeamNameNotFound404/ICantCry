// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HealOther.generated.h"

/**
 * Heal other
 */
UCLASS()
class ICANTCRY_API UBTDecorator_HealOther : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HealOther();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
