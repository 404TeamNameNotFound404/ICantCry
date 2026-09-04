// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_HideNPC.generated.h"


class UCinematicManager;
class AICC_Player;

/**
 * CLASS: UEvent_HideNPC
 * DESCRIPTION: A narrative event that hides all NPCs with a specific Actor Tag
 * behind a black screen (disabling visibility, collision, and ticking).
 * Typically used in OnDialogueEnded to have an NPC exit the scene after a line of dialogue.
 * The World is taken from the Player, so this event also works when it runs as a quest
 * reward, where the Context is a DataAsset with no World.
 */
UCLASS(DisplayName = "Event: Hide NPC")
class ICANTCRY_API UEvent_HideNPC : public UGameplayEvent
{
	GENERATED_BODY()

public:

    /** Actor Tag of the NPC to hide: every actor carrying this tag will be hidden */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName TargetNPCActorTag;

    /** Duration in seconds of the fade to black and of the fade back in */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    /** Seconds of fully black screen before the view opens up again */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    /** Runs the event: finds the CinematicManager and hides every NPC with TargetNPCActorTag */
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
