// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Buff.generated.h"

/**
 * Buff decorator
 */
UCLASS()
class ICANTCRY_API UBTDecorator_Buff : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_Buff();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
