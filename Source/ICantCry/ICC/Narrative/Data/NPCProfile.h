// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "NPCProfile.generated.h"

/**
 * CLASS: UNPCProfile
 * DESCRIPTION: contains identifying data for an npc, used by dialogue assets to display name and portraits
 * this allows multiple dialogue assets to share the same npc identity without duplicating data
 */
UCLASS(BlueprintType)
class ICANTCRY_API UNPCProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** display name shown in dialogue ui for this npc */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText NPCName;

	/** map of emotion tags to portrait textures
	    example: "Emotion.Happy" -> T_NPCNAME_Happy
	    the dialogue line's emotion tag determines which portrait is shown */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> Portraits;
};

	

