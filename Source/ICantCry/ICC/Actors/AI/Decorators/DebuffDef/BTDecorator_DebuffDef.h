// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DebuffDef.generated.h"

/**
 * Debuff defence decorator
 * If True the AI will debuff the Target defence
 */
UCLASS()
class ICANTCRY_API UBTDecorator_DebuffDef : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_DebuffDef();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
