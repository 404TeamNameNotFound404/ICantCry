// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
#include "BTRethinker.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBTRethinker : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTRethinker();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	void OnThinkComplete(UBehaviorTreeComponent* OwnerComp, AICC_AIController* Controller);
	void OnThinkComplete_Internal();
	void StartAttackMinigame(AMob* CurrentMob, AICC_Player* Target, AICC_AIController* Controller);
	void ProcessDecision(EDecision Dec, AMob* CurrentMob, UBlackboardComponent* Board, UBehaviorTreeComponent* OwnerComp, AICC_Player* Target);
	UFUNCTION() void OnMinigameEndedCallback();

	UPROPERTY()
	UBlackboardComponent* BlackBoard;

	UPROPERTY()
	AMob* Current;

	UPROPERTY()
	FDecisionMaker DecisionMaker;

	UPROPERTY()
	UBehaviorTreeComponent* TreeComp;
	
	UPROPERTY()
	EDecision Decision;

	UPROPERTY()
	bool bBusy = false;

	UPROPERTY()
	bool bWaitingForThinkCompletion = false;

	UPROPERTY()
	FTimerHandle DelayHandle;

	UPROPERTY()
	float Delay = 1.5f;
};
