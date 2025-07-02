// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
#include "BTTask_Heal.generated.h"
/**
 * Heal Task
 */
UCLASS()
class ICANTCRY_API UBTTask_Heal : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Heal();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float TimeToHeal = 5.0f;

private:
	
	UPROPERTY()
	UBlackboardComponent* BlackBoard;

	UPROPERTY()
	float Timer = 0.0f;

	UPROPERTY()
	EDecision Decision;
	
};
