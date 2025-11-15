// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "NoteButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoteSelectedSignature, FString, NoteName);

UCLASS()
class ICANTCRY_API UNoteButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
    void SetupNote(const FString& InNoteName); 

	UPROPERTY(BlueprintAssignable)
    FOnNoteSelectedSignature OnNoteSelected;

    UFUNCTION(BlueprintCallable)
    void SetNoteKey(const FString& Key) { NoteKey = Key; }

    UFUNCTION(BlueprintCallable)
    FString GetNoteName() const;
    
    UFUNCTION(BlueprintCallable)
    FString GetNoteKey() const { return NoteKey; }

protected:

    /** Bottone associato alla nota */
    UPROPERTY(meta = (BindWidget))
    UButton* NoteButton;

    /** Testo che mostra il nome della nota */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NoteText;

    UPROPERTY(BlueprintReadOnly, Category = "Note")
    FString NoteName;
 
   

private:

    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY()
    FString CurrentNoteName; 

    UPROPERTY()
    FString NoteKey;
	
};
