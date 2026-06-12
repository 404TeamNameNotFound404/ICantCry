// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "GameplayTagContainer.h"
#include "Event_ChangeLevel.generated.h"


class AICC_Player;
class USceneLoader;

/**
 * CLASS: UEvent_ChangeLevel
 * DESCRIPTION: Manages level transitions, allowing the player to teleport to specific maps 
 * or save the current world state for return trips (e.g., entering a battle arena).
 */
UCLASS(Blueprintable, BlueprintType)
class ICANTCRY_API UEvent_ChangeLevel : public UGameplayEvent
{
	GENERATED_BODY()
	
public:
	/** If enabled, the player will spawn at a designated PlayerStart actor. If disabled, 
     * the system will attempt to restore the player's last saved location. 
     */
    UPROPERTY(EditAnywhere, Category = "Level Transition")
    bool bUsePlayerStart = true;

    /** The unique GameplayTag representing the destination map (e.g., Maps.City). */
    UPROPERTY(EditAnywhere, Category = "Level Transition")
    FGameplayTag LevelTag;

    /** The specific tag of the PlayerStart where the character should appear. */
    UPROPERTY(EditAnywhere, Category = "Level Transition", meta = (EditCondition = "bUsePlayerStart"))
    FGameplayTag PlayerStartTag;

    /** Overridden execution logic for map loading and state saving. */
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
