// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "Event_CompleteQuest.generated.h"


class AICC_Player;
/**
 * 
 */
UCLASS()
class ICANTCRY_API UEvent_CompleteQuest : public UGameplayEvent
{
	GENERATED_BODY()


public:
    // La missione che vogliamo completare
    UPROPERTY(EditAnywhere, Category = "Quest")
    FGameplayTag QuestToComplete;

    // Sovrascriviamo l'esecuzione dell'evento
    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;

	
};
