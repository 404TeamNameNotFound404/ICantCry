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
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

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
};
