// Fill out your copyright notice in the Description page of Project Settings.


#include "../UI/NoteButtonWidget.h"

void UNoteButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentNoteName = TEXT("");

    if (NoteButton)
    {
        NoteButton->OnClicked.AddDynamic(this, &UNoteButtonWidget::OnButtonClicked);
    }
}

void UNoteButtonWidget::SetupNote(const FString &InNoteName)
{
    NoteName = InNoteName;

    if (NoteText)
    {
        
        NoteText->SetText(FText::FromString(InNoteName));
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] SetupNote: Name='%s'"), *NoteName);
   
    }
    
}

void UNoteButtonWidget::OnButtonClicked()
{
    // [DEBUG_START] - Note button click
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === NOTE BUTTON CLICKED ==="));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] NoteName: %s"), *NoteName);
    // [DEBUG_END]

    if (OnNoteSelected.IsBound())
    {
        OnNoteSelected.Broadcast(NoteName); // currentNoteName
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Broadcasted: %s"), *NoteName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ERROR] OnNoteSelected is not bound!"));
    }
}


FString UNoteButtonWidget::GetNoteName() const
{
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] GetNoteName: returning '%s'"), *NoteName);
    return NoteName;
}
