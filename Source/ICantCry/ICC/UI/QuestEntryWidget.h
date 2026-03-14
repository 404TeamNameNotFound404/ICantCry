// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "QuestEntryWidget.generated.h"


/**
 * Rappresenta una singola riga nella lista missioni dello ScrollBox
 */
UCLASS()
class ICANTCRY_API UQuestEntryWidget : public UUserWidget
{
	GENERATED_BODY()


public:
    // Funzione chiamata dalla CharacterUI per inizializzare questa riga
    void SetupQuestEntry(const FQuestProgress& InProgress, class UCharacterUI* InParentUI);

    void UpdateObjectiveDisplay(const FQuestProgress& Details);

protected:
    virtual void NativeConstruct() override;

    // --- Componenti UI (Devono avere lo stesso nome nel Blueprint) ---
    
    UPROPERTY(meta = (BindWidget))
    class UButton* BtnSelect;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextQuestTitle;

    // --- Dati salvati ---
    
    FQuestProgress StoredProgress;
    
    UPROPERTY()
    class UCharacterUI* ParentUI;

    // Funzione chiamata al click del bottone
    UFUNCTION()
    void OnRowClicked();


	
};
