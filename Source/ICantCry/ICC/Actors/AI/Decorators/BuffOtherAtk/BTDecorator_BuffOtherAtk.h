// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_BuffOtherAtk.generated.h"

/**
 * Buff Other AI atk
 */
UCLASS()
class ICANTCRY_API UBTDecorator_BuffOtherAtk : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_BuffOtherAtk();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
