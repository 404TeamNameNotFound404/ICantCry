// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Buff.generated.h"

/**
 * Buff Node Task
 */
UCLASS()
class ICANTCRY_API UBTTask_Buff : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Buff();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
	UPROPERTY()
	UBlackboardComponent* BlackBoard;
	
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	bool bTimerStarted;
};
