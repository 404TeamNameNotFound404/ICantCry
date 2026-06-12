// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

#include "Blueprint/UserWidget.h"


#include "../Mechanics/Core/Data/PlayerStats.h"

#include "../UI/StatsButtonWidget.h"
#include "../UI/QuestEntryWidget.h"

#include "../Narrative/Core/QuestManagerSystem.h"
#include "../Narrative/Data/QuestDefinition.h"
#include "../Narrative/UI/QuestLog_Row.h"

#include "CharacterUI.generated.h"

class AICC_Player;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UCharacterUI : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public: 

	/**
	 * upd the intire UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Character UI") void RefreshUI();

	UFUNCTION(BlueprintCallable, Category = "Quest System") void ClearQuestDetails();


	UFUNCTION(BlueprintCallable, Category = "Quest System") void DisplayQuestDetails(const FQuestProgress& Details);
    
protected:


	// LEFT
	UPROPERTY(meta = (BindWidget))	UScrollBox* MainQuestScrollBox;
	UPROPERTY(meta = (BindWidget))	UScrollBox* SideQuestScrollBox;
	UPROPERTY(meta = (BindWidget))	UTextBlock* TextQuestTitle;
	UPROPERTY(meta = (BindWidget))	UTextBlock* TextQuestDescription;
	UPROPERTY(meta = (BindWidget))	UVerticalBox* VerticalBoxObjectives;
    
	// CENTER
	UPROPERTY(meta = (BindWidget))	UImage* CharacterImage;
	//UPROPERTY(meta = (BindWidget))	UTextBlock* StatsDescription;


	// RIGHT
	UPROPERTY(meta = (BindWidget))	UTextBlock* CharacterLVTop;
	UPROPERTY(meta = (BindWidget))  UProgressBar* ExpBar;
	UPROPERTY(meta = (BindWidget))  UTextBlock* ExpCurrentTextBar; // es  "120 / 450"
	UPROPERTY(meta = (BindWidget))	UStatsButtonWidget* HealthStats;
	UPROPERTY(meta = (BindWidget))	UStatsButtonWidget* AttackStats;
	UPROPERTY(meta = (BindWidget))	UStatsButtonWidget* DefenceStats;
	UPROPERTY(meta = (BindWidget))	UStatsButtonWidget* SpeedStats;
	UPROPERTY(meta = (BindWidget))  UTextBlock* HealthStatDescriptionTxt;
	UPROPERTY(meta = (BindWidget))  UTextBlock* AttackStatDescriptionTxt;
	UPROPERTY(meta = (BindWidget))  UTextBlock* DefenceStatDescriptionTxt;
	UPROPERTY(meta = (BindWidget))  UTextBlock* SpeedStatDescriptionTxt;

	/** data asset containing the player's current stats */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UPlayerStats* Stats;

	UPROPERTY() UICantCryGameInstance* Instance;
	
	// UI
	/** widget class for stat buttons, used if we need to create them dynamically */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class UStatsButtonWidget> StatsButton;

	/** widget class for quest entries in the scroll boxes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Narrative", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UQuestEntryWidget> QuestEntryClass;

	/** widget class for individual objective rows inside quest details */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ObjectiveRowClass;

	/** called when the quest manager system updates, refreshes quest list and details */
	UFUNCTION() void OnQuestSystemUpdated();
    
	/** tag used to identify main quests in the quest type filtering */
	UPROPERTY(EditAnywhere, Category = "Quest System")
    FGameplayTag MainTag;

	/** tag used to identify side quests in the quest type filtering */
    UPROPERTY(EditAnywhere, Category = "Quest System")
    FGameplayTag SideTag;

	/** tag of the currently selected quest, used to preserve selection during updates */
	UPROPERTY(EditAnywhere, Category = "Quest System")
	FGameplayTag CurrentSelectedQuestTag;


private:
	/** refreshes all stat values from the player stats asset */
	UFUNCTION() void UpdateStats();

	/** updates the character level display text */
	UFUNCTION() void UpdateCharacterLevel();

	/** rebuilds the quest list in both main and side scroll boxes */
	UFUNCTION(BlueprintCallable, Category = "Quest") void UpdateQuests();

	/** updates the experience bar fill percentage and text */
	UFUNCTION() void UpdateExpBar();

	/** placeholder for when a quest entry is selected, currently not implemented */
	UFUNCTION() void OnQuestSelected();

	/** helper function to handle stat button selection and description visibility */
    UFUNCTION() void HandleStatButtonClick(UStatsButtonWidget* ButtonToSelect, UTextBlock* DescriptionToShow);

	/** handlers for individual stat button clicks */
	UFUNCTION() void OnHealthStatsClicked();
	UFUNCTION() void OnAttackStatsClicked(); 
	UFUNCTION() void OnDefenceStatsClicked(); 
	UFUNCTION() void OnSpeedStatsClicked(); 

	/** hides all stat description text blocks */
    UFUNCTION() void HideAllStatDescriptions();
    
    /** shows the selected stat description text block */
    UFUNCTION() void ShowStatDescription(UTextBlock* DescriptionToShow);



};
