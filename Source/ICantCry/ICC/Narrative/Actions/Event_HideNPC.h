// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event_HideNPC.generated.h"

class GameplayEvent;
class UCinematicManager;
class AICC_Player;

/**
 * 
 */
UCLASS(DisplayName = "Event: Hide NPC")
class ICANTCRY_API UEvent_HideNPC : public UGameplayEvent
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    FName TargetNPCActorTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float FadeDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float BlackScreenHoldTime = 1.0f;

    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
