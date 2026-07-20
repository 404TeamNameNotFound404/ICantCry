// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_StartDialogue.generated.h"

class UDialogueWidget;
class UDialogueAsset;
class UBarkWidget;
class AICC_Player;

/**
 * CLASS: UEvent_StartDialogue
 * DESCRIPTION: Spawns the dialogue widget and handles player input mode transition.
 */
UCLASS(DisplayName = "Event: Start Dialogue")
class ICANTCRY_API UEvent_StartDialogue : public UGameplayEvent
{
	GENERATED_BODY()

public:
	// La classe Blueprint del Widget da spawnare
	/*UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UDialogueWidget> WidgetClass;*/

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> WidgetClass;

	// Il Data Asset del dialogo da leggere
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UDialogueAsset> DialogueToPlay;

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
