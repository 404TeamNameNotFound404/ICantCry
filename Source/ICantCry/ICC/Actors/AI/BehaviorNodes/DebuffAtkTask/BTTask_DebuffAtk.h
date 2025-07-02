// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DebuffAtk.generated.h"

/**
 * Debuff atk task
 */
UCLASS()
class ICANTCRY_API UBTTask_DebuffAtk : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DebuffAtk();

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
