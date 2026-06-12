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
 * CLASS: UDialogueChoiceButton
 * DESCRIPTION: button widget used for branching dialogue choices
 * each instance represents one possible reply the player can choose
 */
UCLASS()
class ICANTCRY_API UDialogueChoiceButton : public UUserWidget
{
	GENERATED_BODY()

public:

    /** initializes the button with the reply text and the dialogue that follows if chosen */
    void Setup(FText InText, class UDialogueAsset* InNextDialogue, class UDialogueWidget* InParent);

protected:

    /** text block displaying the player's possible reply */
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextChoice;

    /** clickable button area */
    UPROPERTY(meta = (BindWidget))
    class UButton* BtnChoice;

    /** called when the button is clicked, notifies the parent widget of the choice */
    UFUNCTION()
    void OnButtonClicked();

private:

    /** the dialogue asset that will play if this branch is selected */
    UPROPERTY()
    class UDialogueAsset* TargetDialogue;

    /** reference to the main dialogue widget to send the choice back to */
    UPROPERTY()
    class UDialogueWidget* ParentDialogueWidget;
	
};
