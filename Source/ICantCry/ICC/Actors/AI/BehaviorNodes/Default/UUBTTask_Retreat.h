// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "UUBTTask_Retreat.generated.h"

/**
 * To call right AFTER the performed attack 
 */
UCLASS()
class ICANTCRY_API UUUBTTask_Retreat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UUUBTTask_Retreat();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;



	UPROPERTY()
	UBlackboardComponent* BlackBoard;

	UPROPERTY()
	AICC_Player* Target;

	UPROPERTY()
	bool bRetreated = false;

	UPROPERTY()
	bool bTimerStarted = false;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	float RetreatCountdown = 1.0f;

	UPROPERTY()
	AMob* CurrentEmotion;

	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompPtr;

	UPROPERTY()
	bool bTaskFinished = false;

};
