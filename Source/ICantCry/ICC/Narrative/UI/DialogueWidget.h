// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/Data/PlayerProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Fonts/SlateFontInfo.h"
#include "DialogueWidget.generated.h"


class UDialogueAsset;
class UDialogueChoiceButton;
class UICantCryGameInstance;
class UQuestManagerSystem;
class UVerticalBox;
class UImage;
class UButton;
class UTextBlock;
class AICC_Player;


/**
 * CLASS: UDialogueWidget
 * DESCRIPTION: main widget that appears on screen during conversations
 * handles displaying lines, portraits, typewriter effect, and player choices
 */
UCLASS()
class ICANTCRY_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	/** starts a new dialogue sequence with the given asset */
	UFUNCTION(BlueprintCallable, Category = "Narrative")
	void StartDialogue(UDialogueAsset* NewDialogue);

	/** sets whether the current quest can be accepted or declined, affects button visibility at end */
    void SetIsOptional(bool bOptional) { bIsOptionalQuest = bOptional; }	

	/** applies font and color settings from the current dialogue asset to the text widget */
	void ApplyDialogueStyle();

	/** called when a choice button is selected, handles transitioning to next dialogue or closing */
	UFUNCTION() void OnBranchSelected(UDialogueAsset* NextDialogue);

	// temporary delivery data set by gameplay events
	// used for item turn-in quests
    FGameplayTag CurrentRequiredItemTag;
    int32 CurrentAmountRequired;
    FGameplayTag CurrentQuestTag;
    FGameplayTag CurrentObjectiveTag;
    
    /** updates the delivery ui with current progress from quest manager and player inventory */
	void UpdateDeliveryUI();

protected:

	virtual void NativeConstruct() override;

	// --- Widget Bindings ---

	/** displays the npc's name */
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextNPCName;
    
    /** displays the current dialogue line */
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextDialogueContent;
    
    /** shows the npc's portrait based on emotion tag */
    UPROPERTY(meta = (BindWidget)) UImage* NPCFaceImage;
    
    /** button to advance to the next line */
    UPROPERTY(meta = (BindWidget)) UButton* BtnNext;
	
	/** button to deliver items for turn-in quests */
	UPROPERTY(meta = (BindWidget)) UButton* BtnDeliver;
	
	/** text showing delivery progress (items given / items needed) */
	UPROPERTY(meta = (BindWidget)) UTextBlock* TextDeliveryProgress;

    /** accept button for optional quests */
    UPROPERTY(meta = (BindWidget)) UButton *BtnAccept;
    
    /** decline button for optional quests */
    UPROPERTY(meta = (BindWidget)) UButton* BtnDecline;
	
	/** container for branching choice buttons */
	UPROPERTY(meta = (BindWidget)) UVerticalBox* ChoiceContainer;

	// --- Internal Logic ---
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void DisplayNextLine();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnAcceptClicked();
    UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnDeclineClicked();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void EndDialogue();
	UFUNCTION(BlueprintCallable, Category = "Narrative|Dialogue") void OnDeliverClicked();   
	
private:
	/** the dialogue asset currently being played */
	UPROPERTY() TObjectPtr<UDialogueAsset> CurrentDialogue;
	
	/** widget class to spawn for each choice button in branches */
	UPROPERTY(EditAnywhere, Category = "Narrative") TSubclassOf<UDialogueChoiceButton> ChoiceButtonClass;
    
    /** index of the current line being displayed */
    int32 CurrentLineIndex = 0;
    
    /** whether the current quest can be declined, affects end-of-dialogue behavior */
    bool bIsOptionalQuest = false;

	/** timer handle for typewriter effect */
	FTimerHandle TypewriterTimerHandle;
    
    /** full text of current line, used for typewriter reconstruction */
    FString FullTextCurrentLine;
    
    /** current character index for typewriter effect */
    int32 CurrentCharacterIndex;

	/** typewriter effect: adds one character per tick */
    void OnTypewriterTick();
    
    /** skips typewriter animation and shows full line immediately */
    void FinishLineInstantly();
    
    /** creates and displays choice buttons for branching dialogues */
	void ShowBranches();
	
};
