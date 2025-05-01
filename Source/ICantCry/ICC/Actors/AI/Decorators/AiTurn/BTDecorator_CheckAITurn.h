// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckAITurn.generated.h"

/**
 * Keeps track of the AI turn at runtime and if true run the tree
 */
UCLASS()
class ICANTCRY_API UBTDecorator_CheckAITurn : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckAITurn();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
