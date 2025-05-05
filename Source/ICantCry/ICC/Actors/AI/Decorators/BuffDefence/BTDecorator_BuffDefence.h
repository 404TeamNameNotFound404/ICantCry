// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_BuffDefence.generated.h"

/**
 * Used to check whenever AI Buff AI's Defence
 */
UCLASS()
class ICANTCRY_API UBTDecorator_BuffDefence : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_BuffDefence();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
