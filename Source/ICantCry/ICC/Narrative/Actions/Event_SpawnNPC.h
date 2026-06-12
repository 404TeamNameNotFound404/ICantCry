// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event_SpawnNPC.generated.h"

class GameplayEvent;
class UCinematicManager;
class AICC_Player;

/**
 * UGameplayEvents are Data Assets (and therefore don't live in the level), so I can't directly pass them a reference to a point on the map. 
   The solution is to use a Destination Tag.
   In the level, I'll place an invisible actor (e.g., a Target Point), assign it a Tag (e.g., "BlacksmithSpawnPoint"), and the event will move the NPC to that exact point, retriggering it.
 */
UCLASS(DisplayName = "Event: Show Existing NPC")
class ICANTCRY_API UEvent_SpawnNPC : public UGameplayEvent
{
	GENERATED_BODY()
	
public:

    /** Il Tag dell'NPC che vogliamo far comparire */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName TargetNPCActorTag;

    /** Il Tag di un attore nel livello (es. TargetPoint) dove l'NPC apparirà */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName DestinationPointTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
};
