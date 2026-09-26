// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "GameplayTagContainer.h"
#include "Event_PrepareDelivery.generated.h"

class UDialogueWidget;

/**
 * CLASS: UEvent_PrepareDelivery
 * DESCRIPTION: Injects item delivery requirements into the dialogue UI context.
 */
UCLASS(Blueprintable)
class ICANTCRY_API UEvent_PrepareDelivery : public UGameplayEvent
{
	GENERATED_BODY()



public:
    /** item the npc asks for, must be the same ItemTag set on the pickups (e.g. Item.Stone) */
    UPROPERTY(EditAnywhere, Category = "Delivery")
    FGameplayTag ItemTag;

    /** how many items the player must give before Next appears */
    UPROPERTY(EditAnywhere, Category = "Delivery", meta = (ClampMin = "1"))
    int32 Amount = 1;

    /** quest the delivery belongs to (e.g. Quest.ID.Stone) */
    UPROPERTY(EditAnywhere, Category = "Delivery")
    FGameplayTag QuestTag;

    /** objective that counts the delivered items (e.g. Quest.Obj.Stone.Deliver) */
    UPROPERTY(EditAnywhere, Category = "Delivery")
    FGameplayTag ObjectiveTag;

    /** text shown under the Deliver button. you can use:
        {Delivered} items already given, {Required} = Amount, {InInventory} items the player has, {Item} last part of ItemTag (e.g. Stone).
        example: "Stones given to Branches: {Delivered}/{Required}". leave empty to use the default set in WBP_Dialogue */
    UPROPERTY(EditAnywhere, Category = "Delivery", meta = (MultiLine = "true"))
    FText ProgressText;

    /** sends the delivery data to the dialogue widget passed as Context and refreshes its delivery ui */
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
