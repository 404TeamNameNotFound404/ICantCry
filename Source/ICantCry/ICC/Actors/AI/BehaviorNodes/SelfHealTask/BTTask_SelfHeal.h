// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelfHeal.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBTTask_SelfHeal : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelfHeal();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	
	UPROPERTY()
	UBlackboardComponent* Blackboard;

	UPROPERTY()
	bool bTimeStarted = false;
	
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	bool bDebuffHappened = false;
};
