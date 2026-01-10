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

public: 

	/**
	 * upd the intire UI
	 */
	UFUNCTION(BlueprintCallable, Category = "Character UI") void RefreshUI();

protected:


	// LEFT
	// UPROPERTY(meta = (BindWidget))	UButton* MainQuest;
	// UPROPERTY(meta = (BindWidget))	UButton* SideQuest;
	UPROPERTY(meta = (BindWidget))	UScrollBox* ScrollBoxQuest;
	UPROPERTY(meta = (BindWidget))	UTextBlock* QuestsDesctiption;


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

	// UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class UStatsButtonWidget> StatsButton;


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
	UFUNCTION() void UpdateQuests();

	/**
	 * refresh the ui with the new level of the XpBar
	 */
	UFUNCTION() void UpdateExpBar();




	


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
