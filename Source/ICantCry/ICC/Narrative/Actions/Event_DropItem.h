// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_DropItem.generated.h"


class AICC_Player;
class UInteractionComponent;
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

	/** Base distance (cm) in front of the NPC where the object appears */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float SpawnForwardOffset = 80.0f;

	/** Extra margin (cm) added to the space cleared behind the player, in addition to the object's size */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float ExtraClearance = 20.0f;

	/** Maximum distance (cm) of the downward-facing ray used to place the object on the ground */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float GroundTraceDistance = 2000.0f;


	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
