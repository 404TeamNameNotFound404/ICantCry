// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTask_DefaultAtk.generated.h"

/**
 * This is the standard node AI will execute if it's not afflicted by any status
 * 
 */
UCLASS()
class ICANTCRY_API UUBTTask_DefaultAtk : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UUBTTask_DefaultAtk();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY()
	UBlackboardComponent* BlackBoard;

	UPROPERTY()
	bool bBusy = false;

	UPROPERTY()
	FTimerHandle TimerHandle;
};
