// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_SpawnNPC.generated.h"


class UCinematicManager;
class AICC_Player;

/**
 * CLASS: UEvent_SpawnNPC
 * DESCRIPTION: Shows an NPC that already exists in the level, moving it to a destination
 * point behind a black screen. UGameplayEvents are Data Assets and do not live in the level,
 * so they cannot hold a direct reference to a spot on the map: the destination is addressed
 * by Tag instead. Place an invisible actor (e.g. a Target Point) in the level, give it a Tag
 * (e.g. "BlacksmithSpawnPoint"), and the event will move the NPC exactly there.
 */
UCLASS(DisplayName = "Event: Show Existing NPC")
class ICANTCRY_API UEvent_SpawnNPC : public UGameplayEvent
{
	GENERATED_BODY()
	
public:

    /** Actor Tag of the NPC we want to bring into view */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName TargetNPCActorTag;

    /** Tag of an actor in the level (e.g. a Target Point) where the NPC will appear */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName DestinationPointTag;

    /** Duration in seconds of the fade to black and of the fade back in */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    /** Seconds of fully black screen before the view opens up again */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    /** Runs the event: finds the CinematicManager and moves the tagged NPC to the destination point */
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
};
