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
	
	/**
 * refresh the ui with the new level of the XpBar
 */
	UFUNCTION() void UpdateExpBar();

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


	// Data Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UPlayerStats* Stats;

	UPROPERTY() UICantCryGameInstance* Instance;
	
	// UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class UStatsButtonWidget> StatsButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Narrative", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UQuestEntryWidget> QuestEntryClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ObjectiveRowClass;


	UFUNCTION() void OnQuestSystemUpdated();
    

	UPROPERTY(EditAnywhere, Category = "Quest System")
    FGameplayTag MainTag; // Imposta a Quest.Type.Main

    UPROPERTY(EditAnywhere, Category = "Quest System")
    FGameplayTag SideTag; // Imposta a Quest.Type.Side

	UPROPERTY(EditAnywhere, Category = "Quest System")
	FGameplayTag CurrentSelectedQuestTag;


private:

	/**
	 * refresh the ui with the new data
	 */
	UFUNCTION() void UpdateStats();

	/**
	 * refresh the ui with the new characterLvl
	 */
	UFUNCTION() void UpdateCharacterLevel();

	/**
	 * refresh the quest part, of some quest has done or not
	 */
	
	UFUNCTION(BlueprintCallable, Category = "Quest") void UpdateQuests();

	




	


	// Button Event

	/**
	 *  displays the objective texts and the description texts of the selected mission
	 */
	UFUNCTION() void OnQuestSelected();


	/**
    * helper function to manage button selection and description
    */
    UFUNCTION() void HandleStatButtonClick(UStatsButtonWidget* ButtonToSelect, UTextBlock* DescriptionToShow);

	/**
	 *  displays the lv text and the description text of the selected Stat
	 */
	UFUNCTION() void OnHealthStatsClicked();

	/**
	 *  displays the lv text and the description text of the selected Stat
	 */
	UFUNCTION() void OnAttackStatsClicked(); 

	/**
	 *  displays the lv text and the description text of the selected Stat
	 */
	UFUNCTION() void OnDefenceStatsClicked(); 

	/**
	 *  displays the lv text and the description text of the selected Stat
	 */
	UFUNCTION() void OnSpeedStatsClicked(); 



	/**
	 *  Hide all description stast txt
	 */
    UFUNCTION() void HideAllStatDescriptions();
    
    /**
	 * show the selected stat description txt
	 */
    UFUNCTION() void ShowStatDescription(UTextBlock* DescriptionToShow);





};
