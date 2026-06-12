// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event_SpawnNewNPC.generated.h"


class GameplayEvent;
class UCinematicManager;
class AICC_Player;


/**
 * How to use it
   In your dialog data asset, when you add this new event (Event: Spawn NEW NPC), 
   Unreal will display a drop-down menu called NPC Class To Spawn. 
   There, you can select, for example, BP_Blacksmith, and then enter the name (e.g., "ShopPoint") of the Target Point actor where you want 
   it to materialize in the Destination Point Tag field.
 */
UCLASS(DisplayName = "Event: Spawn NEW NPC")
class ICANTCRY_API UEvent_SpawnNewNPC : public UGameplayEvent
{
	GENERATED_BODY()

public:
    /** The class / Blueprint of the NPC we want to create from scratch in the level */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    TSubclassOf<AActor> NPCClassToSpawn;

    /** The Tag of an actor in the level (e.g. TargetPoint) where the NPC will appear */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName DestinationPointTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
