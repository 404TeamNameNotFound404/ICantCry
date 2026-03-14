// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_StartDialogue.generated.h"

class UDialogueWidget;
class UDialogueAsset;


/**
 * CLASSE: UEvent_StartDialogue
 * DESCRIZIONE: Crea e visualizza il Widget del dialogo a schermo.
 */
UCLASS(DisplayName = "Event: Start Dialogue")
class ICANTCRY_API UEvent_StartDialogue : public UGameplayEvent
{
	GENERATED_BODY()

public:
	// La classe Blueprint del Widget da spawnare
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UDialogueWidget> WidgetClass;

	// Il Data Asset del dialogo da leggere
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UDialogueAsset> DialogueToPlay;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
