// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinematicManager.generated.h"


class AWorldCamera;
class ACameraWaypoint;

/**
* Global manager for visual transitions and managing the presence of NPCs in the level.
*/
UCLASS()
class ICANTCRY_API UCinematicManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
  

    /** Hides an existing NPC behind a black screen */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndHideNPC(FName TargetNPCTag, float FadeDuration, float HoldTime);

    /** Makes a (previously hidden) NPC visible and moves it to a specific point in the level */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndShowNPC(FName TargetNPCTag, FName DestinationPointTag, float FadeDuration, float HoldTime);

    /** Spawns a NEW NPC from a class at a specific point in the level during a black screen */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndSpawnNewNPC(TSubclassOf<AActor> NPCClassToSpawn, FName DestinationPointTag, float FadeDuration, float HoldTime);


private:

    void BlockPlayerInput(bool bBlock);
    void PerformCameraFade(float FromAlpha, float ToAlpha, float Duration);

    
   


};
