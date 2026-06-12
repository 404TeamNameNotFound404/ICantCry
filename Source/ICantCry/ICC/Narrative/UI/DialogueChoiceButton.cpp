// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueChoiceButton.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"


void UDialogueChoiceButton::Setup(FText InText, UDialogueAsset* InNextDialogue, UDialogueWidget* InParent)
{
    TargetDialogue = InNextDialogue;
    ParentDialogueWidget = InParent;

    // set the button text if the binding is valid
    if (TextChoice) TextChoice->SetText(InText);
    
    // bind the click event only once during setup
    if (BtnChoice)
    {
        BtnChoice->OnClicked.AddDynamic(this, &UDialogueChoiceButton::OnButtonClicked);
    }
}

void UDialogueChoiceButton::OnButtonClicked()
{
    // tell the main dialogue widget which branch the player picked
    // this will either start a new dialogue or close the widget
    if (ParentDialogueWidget)
    {
        ParentDialogueWidget->OnBranchSelected(TargetDialogue);
    }
}