// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "../Actors/EmotionsData/EmotionsData.h"
#include "../Actors/NoteData/NoteData.h"
#include "../UI/EmotionButtonWidget.h"
#include "../UI/NoteButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "BestiaryUI.generated.h"


class UICantCryGameInstance;

UENUM(BlueprintType)
enum class EEmotionPage : uint8
{
	Stats,      // emotion stats
    Narrative,  // narrative info    
    Count       // tot num of pages
};


UCLASS()
class ICANTCRY_API UBestiaryUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
    void SetupEmotionsData(UEmotionsData* Data);

    UFUNCTION(BlueprintCallable)
    void SetupNoteData(UNoteData* Data);

	// add note (pickup obj)
	UFUNCTION(BlueprintCallable)
    void AddCollectedNote(const FString& NoteKey);

	// check if note is already collected
	UFUNCTION(BlueprintCallable)
    bool HasNote(const FString& NoteKey) const;

    UFUNCTION(BlueprintCallable)
    void UnlockEmotion(EEmotionType Emotion);

    UFUNCTION(BlueprintCallable)
    void NextPage();

    UFUNCTION(BlueprintCallable)
    void PreviousPage();

    //Refresh emotion buttons from GameInstance data 
    UFUNCTION(BlueprintCallable)
    void RefreshNotesFromGameInstance();

    //Refresh emotion buttons from GameInstance data 
    UFUNCTION(BlueprintCallable)
    void RefreshEmotionButtons();

    bool IsOpen() const;

    void SetIsOpen(const bool& Value);

protected:


    //////  LEFT ////// 
    UPROPERTY(meta = (BindWidget))
	UScrollBox* EmotionsList;  // left pannel scroll //UVerticalBox

    //////  RIGHT   ////// 
    UPROPERTY(meta = (BindWidget))
    UScrollBox* NotesList;

    ////// MID  ////// 
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* ContentSwitcher; // it was pageswitcher

    UPROPERTY(meta = (BindWidget))
    UImage* ContentImage; // it was  EmotionImage

    UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText; // it was emotionNameText


    // contentswitcher pages
    UPROPERTY(meta = (BindWidget))
    UWidget* EmotionDetailsPage;

	UPROPERTY(meta = (BindWidget))
    UWidget* NoteDetailsPage;


    // switcher for emotion page (stats/narrative)
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* EmotionPageSwitcher;

	// Specific emotion elements - Stats page
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WeaknessesText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResistancesText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MovesText;
	
	
    // Specific emotion elements - Narrative page
	UPROPERTY(meta = (BindWidget))
    UTextBlock* NarrativeInfoText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BattleBarksText;


    // Specific elements notes
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NoteContentText;

    // Navigation
 
	UPROPERTY(meta = (BindWidget))
    UTextBlock* PageIndicatorText;

    UPROPERTY(meta = (BindWidget))
	UButton* PreviousBtnPage;

    UPROPERTY(meta = (BindWidget))
	UButton* NextBtnPage;


    // Data Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UEmotionsData* EmotionsData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNoteData* NoteData;

    //UI
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class UEmotionButtonWidget> EmotionButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class UNoteButtonWidget> NoteButton;
   

private:

	UFUNCTION()
	void OnEmotionSelected(EEmotionType SelectedEmotion); // Callback pulsanti

    UFUNCTION()
    void OnNoteSelected(FString SelectedNote);

    UFUNCTION()
	void UpdateEmotionDetails(EEmotionType Emotion); 

    UFUNCTION()
	void ChangePage(int32 Direction);

    UFUNCTION()
	void UpdatePageContent(); 

    UFUNCTION()
	void UpdateNotesDisplay(); // upd note   

    UFUNCTION()
    void InitializeSwitchers();

	UPROPERTY()
    EEmotionType CurrentEmotion; // currently selected emotion

	UPROPERTY()
    EEmotionPage CurrentPage;

    UPROPERTY()
    UICantCryGameInstance* ICantCryGameInstance;

    UPROPERTY()
    TMap<EEmotionType, UEmotionButtonWidget*> EmotionButtonMap;

    UPROPERTY()
    bool bIsOpen = false;

    

};




// TO DO :  

// Devi debuggare in gameinstance se lo sblocco funziona dopo la morte del nemico 

// Rifai tutto il debug 