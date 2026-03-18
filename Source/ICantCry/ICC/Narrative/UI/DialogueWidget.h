// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Components/TextBlock.h"
#include "Fonts/SlateFontInfo.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "DialogueWidget.generated.h"


class UDialogueAsset;
class UDialogueChoiceButton;
class UICantCryGameInstance;
class UQuestManagerSystem;

/**
 * CLASSE: UDialogueWidget
 * DESCRIZIONE: Il Widget che appare a schermo durante una conversazione.
 */
UCLASS()
class ICANTCRY_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	// Avvia la sequenza di dialogo
	UFUNCTION(BlueprintCallable, Category = "Narrative")
	void StartDialogue(UDialogueAsset* NewDialogue);

	// Funzione per impostare se la missione corrente permette il rifiuto
    void SetIsOptional(bool bOptional) { bIsOptionalQuest = bOptional; }	

	void ApplyDialogueStyle();

	// Funzione chiamata quando si preme un bottone di scelta
	UFUNCTION() void OnBranchSelected(UDialogueAsset* NextDialogue);


	// Dati temporanei della consegna impostati dall'evento
	// per consegnare gli oggetti quest
    FGameplayTag CurrentRequiredItemTag;
    int32 CurrentAmountRequired;
    FGameplayTag CurrentQuestTag;
    FGameplayTag CurrentObjectiveTag;
	void UpdateDeliveryUI();

protected:
	virtual void NativeConstruct() override;

	// --- Widget Binding ---

	UPROPERTY(meta = (BindWidget)) UTextBlock* TextNPCName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextDialogueContent;
    UPROPERTY(meta = (BindWidget)) UImage* NPCFaceImage;
    UPROPERTY(meta = (BindWidget)) UButton* BtnNext;
	UPROPERTY(meta = (BindWidget)) UButton* BtnDeliver; // Bottone per dare l'oggetto
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextDeliveryProgress; // txt per il progresso degli oggetti dati


    //bottoni per la scelta
    UPROPERTY(meta = (BindWidget)) UButton *BtnAccept;
    UPROPERTY(meta = (BindWidget)) UButton* BtnDecline;
	UPROPERTY(meta = (BindWidget))UVerticalBox* ChoiceContainer;

	// --- Logica Interna ---
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void DisplayNextLine();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnAcceptClicked();
    UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnDeclineClicked();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void EndDialogue();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnDeliverClicked();   
	
	
private:

	UPROPERTY() TObjectPtr<UDialogueAsset> CurrentDialogue;
	
	// Classe del widget bottone da spawnare
	UPROPERTY(EditAnywhere, Category = "Narrative") TSubclassOf<UDialogueChoiceButton> ChoiceButtonClass;
    
    int32 CurrentLineIndex = 0;
    bool bIsOptionalQuest = false;

	FTimerHandle TypewriterTimerHandle;
    FString FullTextCurrentLine;
    int32 CurrentCharacterIndex;

	// Typewriter Effect
    void OnTypewriterTick();
    void FinishLineInstantly(); // Per saltare l'animazione se il player clicca "Next"
	void ShowBranches();


	
};
