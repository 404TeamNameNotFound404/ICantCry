// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "BTTask_Flee.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBTTask_Flee : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Flee();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
	UPROPERTY()
	UBlackboardComponent* Blackboard;
	
	UPROPERTY()
	AMob* Current;
	
	UPROPERTY()
	int32 Attempts = 0;
	
	UPROPERTY()
	UBehaviorTreeComponent* OwnerRef;
	
	void Run();
	void FailFlee();
};
