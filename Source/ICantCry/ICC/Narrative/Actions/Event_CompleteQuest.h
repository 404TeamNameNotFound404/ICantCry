// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "Event_CompleteQuest.generated.h"


class AICC_Player;

/**
 * CLASS: UEvent_CompleteQuest
 * DESCRIPTION: Directly triggers the completion of a specific quest via the QuestManager.
 */
UCLASS()
class ICANTCRY_API UEvent_CompleteQuest : public UGameplayEvent
{
	GENERATED_BODY()


public:
    /** tag of the quest to be finalized. */
    UPROPERTY(EditAnywhere, Category = "Quest")
    FGameplayTag QuestToComplete;


    /** If true, completes the quest even if the objectives are not met */
    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bForceComplete = false;
   
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;

	
};
