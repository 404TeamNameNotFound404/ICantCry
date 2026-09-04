// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_SpawnNewNPC.generated.h"


class UCinematicManager;
class AICC_Player;


/**
 * CLASS: UEvent_SpawnNewNPC
 * DESCRIPTION: Creates a brand new NPC in the level behind a black screen, at a destination
 * point addressed by Tag. How to use it: add this event (Event: Spawn NEW NPC) to a dialogue
 * data asset, pick the Blueprint to create in NPC Class To Spawn (e.g. BP_Blacksmith), then
 * type the Tag of the Target Point where it should materialise in Destination Point Tag
 * (e.g. "ShopPoint").
 */
UCLASS(DisplayName = "Event: Spawn NEW NPC")
class ICANTCRY_API UEvent_SpawnNewNPC : public UGameplayEvent
{
	GENERATED_BODY()

public:

    /** The class / Blueprint of the NPC we want to create from scratch in the level */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    TSubclassOf<AActor> NPCClassToSpawn;

    /** Tag of an actor in the level (e.g. a Target Point) where the NPC will appear */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName DestinationPointTag;

    /** Duration in seconds of the fade to black and of the fade back in */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    /** Seconds of fully black screen before the view opens up again */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    /** Runs the event: finds the CinematicManager and spawns the NPC at the destination point */
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
