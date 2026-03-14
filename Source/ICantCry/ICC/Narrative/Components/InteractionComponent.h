// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class UDialogueAsset;
class UQuestDefinition;
class UDialogueWidget;
class AICC_Player;



USTRUCT(BlueprintType)
struct FQuestDialogueChain
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Quest")
    TObjectPtr<UQuestDefinition> Quest;

    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bIsOptional = false; // Se TRUE, appariranno i tasti Accetta/Rifiuta

    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> StartDialogue; // Primo incontro

    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> InProgressDialogue; // Durante la quest

    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> CompletedDialogue; // Consegna
};



/**
 * CLASSE: UInteractionComponent
 * DESCRIZIONE: Componente da aggiungere agli NPC per renderli cliccabili.
 */
UCLASS(ClassGroup=(Narrative), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	// --- CONFIGURAZIONE DIALOGHI ---
	
	// lista sequenziale
    UPROPERTY(EditAnywhere, Category = "Narrative|Config")
    TArray<FQuestDialogueChain> QuestChain;

    // Dialogo mostrato se non ci sono missioni disponibili o se tutte sono finite
    UPROPERTY(EditAnywhere, Category = "Narrative|Dialogue")
    TObjectPtr<UDialogueAsset> FinalDefaultDialogue;


	// Classe del Widget da usare per il dialogo
	UPROPERTY(EditAnywhere, Category = "Narrative|Config")
	TSubclassOf<class UDialogueWidget> DialogueWidgetClass;


	// Funzione principale chiamata dal Player
	void TriggerInteraction(AICC_Player* Player);
};
