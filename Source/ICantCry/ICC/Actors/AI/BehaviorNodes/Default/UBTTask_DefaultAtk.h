// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
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
	void OnThinkComplete_Internal();
	void OnThinkComplete(UBehaviorTreeComponent* OwnerComp, AICC_AIController* Controller);
	void ProcessDecision(EDecision Dec, AMob* Current, UBlackboardComponent* Board, UBehaviorTreeComponent* OwnerComp, AICC_Player* Target);
	UFUNCTION()
	void OnMinigameEndedCallback();
	void StartAttackMinigame(AMob* Current, AICC_Player* Target, AICC_AIController* Controller);

	UPROPERTY()
	UBlackboardComponent* BlackBoard;

	UPROPERTY()
	FDecisionMaker DecisionMaker;

	UPROPERTY()
	bool bWaitingForThinkCompletion = false;

	UPROPERTY()
	bool bBusy = false;
	
	UPROPERTY()
	FTimerHandle DelayHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta=(AllowPrivateAccess=true), Category="ICC")
	float Delay = 2.f;

	UPROPERTY()
	EDecision Decision;

	UPROPERTY()
	bool bBuffed = false;

	UPROPERTY()
	bool bThought = false;

	UPROPERTY()
	float buffTimer = 0.0f;

	UPROPERTY()
	AMob* CurrentMob;

	UPROPERTY()
	UBehaviorTreeComponent* TreeComp;
};
