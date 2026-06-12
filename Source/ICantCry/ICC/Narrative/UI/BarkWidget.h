// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarkWidget.generated.h"


class UDialogueAsset;
class UButton;
class UTextBlock;
class UImage;


/**
* CLASS: UBarkWidget
* is activated when the player interacts with objects/notes in the game world.
*/
UCLASS()
class ICANTCRY_API UBarkWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    /** Avvia la sequenza di bark. Blocca il player finché non finisce. */
    UFUNCTION(BlueprintCallable, Category = "Bark")
    void StartBark(UDialogueAsset* NewBark);

protected:
    virtual void NativeConstruct() override;

    // --- Bindings (gli stessi nomi vanno usati nel WBP) ---
    UPROPERTY(meta = (BindWidget))            UTextBlock* TextDialogueContent;
    UPROPERTY(meta = (BindWidget))            UButton* BtnNext;
    UPROPERTY(meta = (BindWidgetOptional))    UTextBlock* TextPlayerName;
    UPROPERTY(meta = (BindWidgetOptional))    UImage* PlayerFaceImage;

    UFUNCTION(BlueprintCallable, Category = "Bark") void DisplayNextLine();
    UFUNCTION(BlueprintCallable, Category = "Bark") void EndBark();

private:
    UPROPERTY() TObjectPtr<UDialogueAsset> CurrentBark;
    int32 CurrentLineIndex = 0;

    FTimerHandle TypewriterTimerHandle;
    FString FullTextCurrentLine;
    int32 CurrentCharacterIndex = 0;

    void ApplyStyle();
    void OnTypewriterTick();
    void FinishLineInstantly();
	
};
