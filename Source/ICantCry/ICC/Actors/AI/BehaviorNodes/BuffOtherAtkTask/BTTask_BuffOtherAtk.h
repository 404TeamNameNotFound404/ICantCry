// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BuffOtherAtk.generated.h"

/**
 * Buff other AIs attack task
 */
UCLASS()
class ICANTCRY_API UBTTask_BuffOtherAtk : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BuffOtherAtk();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
		
	UPROPERTY()
	UBlackboardComponent* BlackBoard;
	
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	bool bTimerStarted;
};
