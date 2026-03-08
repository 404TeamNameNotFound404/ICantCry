// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueChoiceButton.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"


void UDialogueChoiceButton::Setup(FText InText, UDialogueAsset* InNextDialogue, UDialogueWidget* InParent)
{
    TargetDialogue = InNextDialogue;
    ParentDialogueWidget = InParent;

    if (TextChoice) TextChoice->SetText(InText);
    
    if (BtnChoice)
    {
        BtnChoice->OnClicked.AddDynamic(this, &UDialogueChoiceButton::OnButtonClicked);
    }
}

void UDialogueChoiceButton::OnButtonClicked()
{
    if (ParentDialogueWidget)
    {
        // Notifichiamo al widget principale che questa scelta è stata fatta
        ParentDialogueWidget->OnBranchSelected(TargetDialogue);
    }
}