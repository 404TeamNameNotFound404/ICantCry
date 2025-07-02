// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IdCondition.generated.h"

/**
 * Evaluate Id
 */
UCLASS()
class ICANTCRY_API UBTDecorator_IdCondition : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IdCondition();

protected:
	/**
	 * The number you want to compare with the Id
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evaluation", meta = (AllowPrivateAccess = "true"))
	int Evaluation;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IdKey;

};
