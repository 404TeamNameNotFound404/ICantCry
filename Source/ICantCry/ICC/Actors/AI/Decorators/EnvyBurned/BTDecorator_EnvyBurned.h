// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_EnvyBurned.generated.h"

/**
 *	Envy burned check
 */
UCLASS()
class ICANTCRY_API UBTDecorator_EnvyBurned : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_EnvyBurned();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
