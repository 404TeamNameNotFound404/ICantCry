// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_CollectItem.generated.h"

/**
 * CLASS: UEvent_CollectItem
 * DESCRIPTION: Forces the collection of a physical world item through interaction logic.
 */
UCLASS(DisplayName = "Event: Collect Item")
class ICANTCRY_API UEvent_CollectItem : public UGameplayEvent
{
	GENERATED_BODY()


public:

	/** triggers the collect logic on the actor provided by the context. */
	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
