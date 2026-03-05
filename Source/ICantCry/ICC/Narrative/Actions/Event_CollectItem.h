// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_CollectItem.generated.h"

/**
 * CLASSE: UEvent_CollectItem
 * DESCRIZIONE: Triggera la raccolta di un oggetto fisico nel mondo.
 */
UCLASS(DisplayName = "Event: Collect Item")
class ICANTCRY_API UEvent_CollectItem : public UGameplayEvent
{
	GENERATED_BODY()


public:
	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
