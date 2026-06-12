// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayEvent.generated.h"


class AICC_Player;

/**
 * CLASS: UGameplayEvent
 * DESCRIPTION: abstract base class for all narrative actions
 * allows designers to create custom events (like "open door", "start cutscene")
 * that can be placed inside quests or dialogues as data assets
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ICANTCRY_API UGameplayEvent : public UObject
{
	GENERATED_BODY()

	public:
	/**
	 * executes the event's logic
	 * @param Player: the player who triggers or receives the event
	 * @param Context: the object that caused the event (eg the npc or a trigger)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Narrative")
	void ExecuteEvent(AICC_Player* Player, UObject* Context);

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context);
	
};
