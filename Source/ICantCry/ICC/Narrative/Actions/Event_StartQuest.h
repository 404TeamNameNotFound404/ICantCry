// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "Event_StartQuest.generated.h"

/**
 * CLASS: UEvent_StartQuest
 * DESCRIPTION: Registers a new quest into the player's active quest list.
 */
UCLASS(DisplayName = "Event: Start Quest")
class ICANTCRY_API UEvent_StartQuest : public UGameplayEvent
{
	GENERATED_BODY()


public:
	// The mission this event is meant to begin
	UPROPERTY(EditAnywhere, Category = "Quest")
	TObjectPtr<UQuestDefinition> QuestToStart;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
