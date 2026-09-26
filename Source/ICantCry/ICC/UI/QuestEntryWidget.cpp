// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestEntryWidget.h"
#include "ICantCry/ICC/UI/CharacterUI.h" 
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"



void UQuestEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!BtnSelect)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestEntryWidget - BtnSelect mancante nel Blueprint %s"), *GetClass()->GetName());
        return;
    }

    // NativeConstruct runs again every time the widget is re-added to a parent:
    // AddUnique avoids a double binding that would fire the toggle twice (open + close in the same click)
    BtnSelect->OnClicked.AddUniqueDynamic(this, &UQuestEntryWidget::OnRowClicked);
}

void UQuestEntryWidget::SetupQuestEntry(const FQuestProgress& InProgress, UCharacterUI* InParentUI)
{
    StoredProgress = InProgress;
    ParentUI = InParentUI;

    if (!ParentUI)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget::SetupQuestEntry - ParentUI NULL, il click non fara' nulla"));
    }

    if (TextQuestTitle && InProgress.QuestDef)
    {
        TextQuestTitle->SetText(InProgress.QuestDef->Title);
    }
}

void UQuestEntryWidget::UpdateObjectiveDisplay(const FQuestProgress& Details)
{
}

void UQuestEntryWidget::OnRowClicked()
{
    if (!ParentUI || !StoredProgress.QuestDef)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestEntryWidget::OnRowClicked - ParentUI o QuestDef NULL, click ignorato"));
        return;
    }

    // only the id is passed: the Character UI decides open/close and reads fresh data from the manager
    ParentUI->ToggleQuestDetails(StoredProgress.QuestDef->QuestID);
}
