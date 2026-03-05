// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Components/TextBlock.h"
#include "Fonts/SlateFontInfo.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "DialogueWidget.generated.h"


class UDialogueAsset;

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

protected:
	virtual void NativeConstruct() override;

	// --- Widget Binding ---

	UPROPERTY(meta = (BindWidget)) UTextBlock* TextNPCName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextDialogueContent;
    UPROPERTY(meta = (BindWidget)) UImage* NPCFaceImage;
    UPROPERTY(meta = (BindWidget)) UButton* BtnNext;

    //bottoni per la scelta
    UPROPERTY(meta = (BindWidget)) UButton *BtnAccept;
    UPROPERTY(meta = (BindWidget)) UButton* BtnDecline;

	// --- Logica Interna ---
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void DisplayNextLine();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnAcceptClicked();
    UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnDeclineClicked();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void EndDialogue();
	

private:

	UPROPERTY() TObjectPtr<UDialogueAsset> CurrentDialogue;
    int32 CurrentLineIndex = 0;
    bool bIsOptionalQuest = false;
	
};
