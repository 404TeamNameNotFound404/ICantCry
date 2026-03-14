// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestEntryWidget.h"
#include "ICantCry/ICC/UI/CharacterUI.h" // Includi la tua UI principale
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"



void UQuestEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BtnSelect)
    {
        BtnSelect->OnClicked.AddDynamic(this, &UQuestEntryWidget::OnRowClicked);
    }
}

void UQuestEntryWidget::SetupQuestEntry(const FQuestProgress& Details, UCharacterUI* InParentUI)
{
    StoredProgress = Details;
    ParentUI = InParentUI;

    if (TextQuestTitle && Details.QuestDef)
    {
        TextQuestTitle->SetText(Details.QuestDef->Title);
    }

}

void UQuestEntryWidget::OnRowClicked()
{
    if (ParentUI && StoredProgress.QuestDef)
    {
        // Notifichiamo alla CharacterUI che questa è la missione selezionata.
        // Sarà la CharacterUI a riempire la sua VerticalBox degli obiettivi.
        ParentUI->DisplayQuestDetails(StoredProgress);
    }
}