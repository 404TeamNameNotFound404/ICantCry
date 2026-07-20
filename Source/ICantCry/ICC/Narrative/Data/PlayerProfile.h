// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h" 
#include "PlayerProfile.generated.h"

/**
 * CLASS: UPlayerProfile
 * DESCRIPTION: contains identifying data for player, used by dialogue assets to display name and portraits
 * to allow him to have active lines of dialogue.
 */
UCLASS(BlueprintType)
class ICANTCRY_API UPlayerProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** display name shown in dialogue ui for the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText PlayerName;

	/** map of emotion tags to portrait textures for the player */
	/** map of emotion tags to portrait textures
	    example: "Emotion.Happy" -> T_Player_Happy
	    the dialogue line's emotion tag determines which portrait is shown */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> Portraits;
	
};
