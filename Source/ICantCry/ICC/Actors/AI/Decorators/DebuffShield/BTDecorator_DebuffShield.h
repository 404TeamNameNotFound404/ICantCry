// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DebuffShield.generated.h"

/**
 * Debuff shield current AI
 */
UCLASS()
class ICANTCRY_API UBTDecorator_DebuffShield : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_DebuffShield();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
