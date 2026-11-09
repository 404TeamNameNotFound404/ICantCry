// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BasePickup.generated.h"

class AICC_Player;
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

	/** called when the player collects the item: stores it in the inventory and/or progresses the quest, then destroys the actor */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Collect(AICC_Player* Player);

protected:

	/** quest this item belongs to (e.g. Quest.ID.Stone). leave empty if picking it up should not progress any quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag TargetQuestTag;

	/** objective that progresses when the item is PICKED UP (e.g. Quest.Obj.Stone.PickUp).
		leave empty if the quest only has a Deliver objective: the item will just go in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag TargetObjectiveTag;

	/** if true the item goes in the persistent inventory of the GameInstance. must be true if a quest asks to DELIVER it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bShouldBeStored = true;

	/** what item this is for the inventory (e.g. Item.Stone). it is the tag the delivery checks and removes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "bShouldBeStored"))
	FGameplayTag ItemTag;

	/** how many units are added to the inventory and to the pick up objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ClampMin = "1"))
	int32 AmountToAdd = 1;
};
