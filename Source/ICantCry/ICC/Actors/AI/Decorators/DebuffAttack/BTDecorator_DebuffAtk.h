// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DebuffAtk.generated.h"

/**
 * Debuff Player Attack Decorator
 */
UCLASS()
class ICANTCRY_API UBTDecorator_DebuffAtk : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_DebuffAtk();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
