// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_DropItem.generated.h"

/**
 * CLASS: UEvent_DropItem
 * DESCRIPTION: Spawns an actor class in front of the current speaker/context actor.
 */
UCLASS(DisplayName = "Event: Drop Item")
class ICANTCRY_API UEvent_DropItem : public UGameplayEvent
{
	GENERATED_BODY()


public:
	/** class of the actor to spawn. (es. BP_stone)*/ 
	UPROPERTY(EditAnywhere, Category = "Drop")
	TSubclassOf<AActor> ItemClass;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
