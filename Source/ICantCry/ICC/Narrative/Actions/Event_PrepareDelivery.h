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
    UPROPERTY(EditAnywhere) FGameplayTag ItemTag;
    UPROPERTY(EditAnywhere) int32 Amount;
    UPROPERTY(EditAnywhere) FGameplayTag QuestTag;
    UPROPERTY(EditAnywhere) FGameplayTag ObjectiveTag;

    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
