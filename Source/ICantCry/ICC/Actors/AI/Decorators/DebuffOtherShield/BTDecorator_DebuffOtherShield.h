// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DebuffOtherShield.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBTDecorator_DebuffOtherShield : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_DebuffOtherShield();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
