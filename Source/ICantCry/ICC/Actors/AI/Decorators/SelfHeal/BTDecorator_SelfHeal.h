// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SelfHeal.generated.h"

/**
 * Self heal
 */
UCLASS()
class ICANTCRY_API UBTDecorator_SelfHeal : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_SelfHeal();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
