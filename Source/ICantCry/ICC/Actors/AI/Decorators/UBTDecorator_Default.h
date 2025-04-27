// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "UBTDecorator_Default.generated.h"

/**
 * Default decorator is used when the other conditions are not met so the AI will
 * move with the default pattern
 */
UCLASS(Blueprintable)
class ICANTCRY_API UUBTDecorator_Default : public UBTDecorator
{
	GENERATED_BODY()

public:
	UUBTDecorator_Default();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	bool HasAnyStateEnabled(UBehaviorTreeComponent& OwnerComp) const;
};
