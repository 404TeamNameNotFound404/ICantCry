// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_FreezedUp.generated.h"

/**
 * Decorator For freezed up check
 */
UCLASS()
class ICANTCRY_API UBTDecorator_FreezedUp : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_FreezedUp();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
