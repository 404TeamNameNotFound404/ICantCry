// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "DialogueChoiceButton.generated.h"

class UDialogueWidget; 
class UDialogueAsset;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UDialogueChoiceButton : public UUserWidget
{
	GENERATED_BODY()

public:
    // Funzione per inizializzare il bottone
    void Setup(FText InText, class UDialogueAsset* InNextDialogue, class UDialogueWidget* InParent);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextChoice;

    UPROPERTY(meta = (BindWidget))
    class UButton* BtnChoice;

    UFUNCTION()
    void OnButtonClicked();

private:
    UPROPERTY()
    class UDialogueAsset* TargetDialogue;

    UPROPERTY()
    class UDialogueWidget* ParentDialogueWidget;
	
};
