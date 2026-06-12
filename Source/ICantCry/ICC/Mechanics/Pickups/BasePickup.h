// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BasePickup.generated.h"


class UICantCryGameInstance;


/**
 * CLASS: ABasePickup
 * DESCRIPTION: Base class for all world items that can be collected. 
 * Handles inventory storage and quest progress updates.
 */

UCLASS()
class ICANTCRY_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:
	
	/** * Main interaction function called when the player collects or interacts with the item.
	 * Processes inventory logic and quest updates.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Collect(AICC_Player* Player);

protected:

	// --- TAGS FOR THE MISSION SYSTEM ---
    
    /** The GameplayTag of the quest this item is linked to. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetQuestTag;

	/** The specific objective tag that should progress when this item is picked up (e.g., Quest.Obj.Gathers). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetObjectiveTag;

    /** If enabled, the item will be physically added to the persistent inventory in the GameInstance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bShouldBeStored = true;

    /** The unique identifier for this item type (e.g., Item.Stone). Required if bShouldBeStored is true. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "bShouldBeStored"))
    FGameplayTag ItemTag;

	/** How many units of this item to add to the inventory or quest progress. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 AmountToAdd = 1;

};
