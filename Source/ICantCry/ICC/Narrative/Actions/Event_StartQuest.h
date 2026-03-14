// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "Event_StartQuest.generated.h"

/**
 * CLASSE: UEvent_StartQuest
 * DESCRIZIONE: Un evento che aggiunge una missione specifica al diario del giocatore.
 */
UCLASS(DisplayName = "Event: Start Quest")
class ICANTCRY_API UEvent_StartQuest : public UGameplayEvent
{
	GENERATED_BODY()


public:
	// La missione che questo evento deve far iniziare
	UPROPERTY(EditAnywhere, Category = "Quest")
	TObjectPtr<UQuestDefinition> QuestToStart;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
