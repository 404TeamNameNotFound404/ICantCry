// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_DropItem.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Event: Drop Item")
class ICANTCRY_API UEvent_DropItem : public UGameplayEvent
{
	GENERATED_BODY()


public:
	// La classe dell'oggetto da spawnare (es. BP_Spada)
	UPROPERTY(EditAnywhere, Category = "Drop")
	TSubclassOf<AActor> ItemClass;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
