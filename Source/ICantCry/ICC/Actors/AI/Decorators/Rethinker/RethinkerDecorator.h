// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "RethinkerDecorator.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API URethinkerDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	URethinkerDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
