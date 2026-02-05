// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerController.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "../Mechanics/TurnSystem/Core/BattleHandler.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/CircularBulletBuffer.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/RevolverSlot.h"
#include "../Inventory/Inventory.h"
#include "../Inventory/BulletIconWidget.h"
#include "ICantCry/ICC/Mechanics/UI/APBar/APBar.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/GameOver/GameOverVisualizer.h"
#include "ICantCry/ICC/Mechanics/UI/BulletDisplay/BulletDisplayer.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/Victory/VictoryVisualizer.h"
#include "ICantCry/ICC/Mechanics/UI/DecisionDisplayer/DecisionDisplayer.h"
#include "BattleHUD.generated.h"

/**
 * 
 */


UCLASS()
class ICANTCRY_API UBattleHUD : public UUserWidget
{
	GENERATED_BODY()

	public:
  
    virtual void NativeConstruct() override;

	// chiama le variabili esattamente come chiami gli oggetti nell'editor
public:

	// Canvas Panels
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasFirstReloadMagazine; 
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasMainBattlePanel; 
    UPROPERTY(meta = (BindWidget)) UCanvasPanel* CanvasMiniGames;
	UPROPERTY(meta = (BindWidget)) USizeBox* MinigameSlot;
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasAmmoSelection;
    UPROPERTY(meta = (BindWidget)) UCanvasPanel* CanvasStatus;

    // Action Buttons
	UPROPERTY(meta = (BindWidget)) UAPBar* Bar;
    UPROPERTY(meta = (BindWidget)) UButton* Shoot;
    UPROPERTY(meta = (BindWidget)) UButton* ApIncreaseOnShoot;
	UPROPERTY(meta = (BindWidget)) UButton* ApDecreaseOnShoot;
	 UPROPERTY(meta = (BindWidget)) UButton* ShootBoost;
    UPROPERTY(meta = (BindWidget)) UButton* Focus;
    UPROPERTY(meta = (BindWidget)) UButton* Reload;
    UPROPERTY(meta = (BindWidget)) UButton* Pass;
    UPROPERTY(meta = (BindWidget)) UButton* ConfirmButton;
	UPROPERTY(meta = (BindWidget)) UButton* ConfirmReloadBullet;
	UPROPERTY(meta = (BindWidget)) UButton* EngageBtn;
	UPROPERTY(meta=(BindWidget)) UDecisionDisplayer* DecisionDisplayer;

    // Status Bars
    UPROPERTY(meta = (BindWidget)) UProgressBar* PlayerHealth;
    UPROPERTY(meta = (BindWidget)) UProgressBar* APBar;

    // Ammo Display
    UPROPERTY(meta = (BindWidget)) UImage* Magazine;
	UPROPERTY(meta=(BindWidget)) UTextBlock* OutOfBulletTxt;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_1;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_2;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_3;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_4;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_5;
    // UPROPERTY(meta = (BindWidget)) UImage* Ammo_6;

    // Targetting
    UPROPERTY(meta = (BindWidget)) UImage* Crosshair;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StatusText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_1;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText_3;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_3;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* BulletName; 
	UPROPERTY(meta = (BindWidget)) UTextBlock* Quantity; 
	UPROPERTY(meta = (BindWidget)) UTextBlock* QuantityTotal;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;

    // Bullet Selection
    UPROPERTY(meta = (BindWidget)) UImage* AmmoSelectionIndicator;
    UPROPERTY(meta = (BindWidget)) TArray<UImage*> BulletIcons;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText_2; // Bullet name
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_2; // Bullet name
 
    // Bullet Data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
    TArray<TSubclassOf<UBulletData>> AvailableBulletTypes;
	UPROPERTY() TArray<URevolverSlot*> RevolverSlots;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot0;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot1;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot2;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot3;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot4;
	UPROPERTY(meta = (BindWidget)) URevolverSlot* RevolverSlot5;

	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_1;
	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_2;
	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_3;
	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_4;
	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_5;
	UPROPERTY(meta = (BindWidget)) UImage* PistolMagazine_6;

	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet0;
	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet1;
	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet2;
	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet3;
	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet4;
	UPROPERTY(meta = (BindWidget)) UMagazineBullet* MagazineBullet5;
	
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* CanvasBulletStats;

	UPROPERTY() TArray<UImage*> PistolMagazines;

	UPROPERTY()
	TArray<UBulletData*> LoadedBulletData;

	UPROPERTY() TArray<UMagazineBullet*> MagazineBullets;
	
	UBulletDisplayer* GetBulletDisplayer() const;
	
    UFUNCTION(BlueprintCallable)void ScrollBulletSelection(float ScrollValue);
	void UpdateTarget();
	void ScrollTargetSelection(float ScrollValue);

	void SetSelectTarget(const bool& Enable);
	bool GetSelectTarget() const;

	AMob* GetCurrentPlayingEmotion() const;
	AMob* GetSelectedEmotion() const;
	AICC_Actor* GetSelectedActor() const;

	UBulletSelector* GetBulletSelector() const;
	UBulletSelector* GetHoveredSelectedBullet() const;
	void SetHoveredSelectedBullet(UBulletSelector* Hovered);
	
	/**
	 * Proceed to the battle phase
	 */
	UFUNCTION()
	void Engage();

	void ShowHUD();
	bool IsShootFired() const;
	bool IsBulletSelectionOver() const;
	bool IsSelectingTarget() const;
	void SetIsSelectingTarget(const bool& Enable);
	void ShowInfo() const;
	void HideInfo() const;
	bool IsReadyToBattle() const;
	ABattleHandler* GetBattleHandler() const;
	UCircularBulletBuffer* GetCircularBulletBuffer() const;
	void SetCurrentPlayingEmotion(AMob* Current);
	UBulletData* GetCurrentBulletData() const;

	void RestoreHealth();
	/**
	 *Reset Health back to full
	 */
	void ResetHealth();

	void SpawnVisualizer();
	void DisplayVictoryVisualizer();

	void SpawnGameOverVisualizer();
	void DisplayGameOverVisualizer();

	UVictoryVisualizer* GetVictoryVisualizer() const;

	void SetBulletSetupFinished(const bool& Value);

	/**
	 * this method bind with the quantity text box in battle hud blueprint to
	 * update the quantity at runtime while clicking
	 * @return runtime selected bullet quantity
	 */
	UFUNCTION(BlueprintPure) FText GetHoveredBulletQuantity();

	UFUNCTION(BlueprintPure) FText UpdateTargetSelectionInfos(); // TargetNameText Bind
	UFUNCTION(BlueprintPure) FText UpdateEnemyName(); // TargetText Bind

	/**
	 * Update the ap after the attack is over
	 */
	void UpdateAp();
	void SetApAccumulator(const int& Value);

	FBullet* GetCurrentSelectedBullet() const;
	void DisableButtonsDuringShooting();
	void EnableButtonsAfterShooting();

	void RefreshBulletMagazine();
	void RefreshPistolMagazine();
	void ResetPistolMagazine();
	void ResetAp();
	void RequestBulletPreparation();

	/**
	 * Reset the hud and all it's call at game over's retry
	 */
	void Reset(const TMap<TEnumAsByte<EBulletType>, FBullet>& Bullets);

private:

    UPROPERTY() ABattleHandler*  BattleHandler = nullptr;


    // Game State
    UPROPERTY() int32 CurrentAP = 0;
	UPROPERTY() int32 ApAccumulator = 0;
	UPROPERTY() int32 BaseCost = 1;

    int32 CurrentEnemyIndex = 0;
    int32 SelectedBulletIndex = 0;
    int32 CurrentRevolverSlot = 0;
    UPROPERTY(EditDefaultsOnly, Category = "Bullets") int32 MaxRevolverSlots = 6;
	
    UPROPERTY() UBulletData* CurrentBulletData;
	
	UPROPERTY()
	UCircularBulletBuffer* RevolverBuffer; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Bullets")
	TSubclassOf<UBulletDisplayer> BulletDisplayerClass;
	
	UPROPERTY() FInventory Inventory;

	// UPROPERTY(meta = (BindWidget))
	// UImage* Bullet_1;
	// UPROPERTY(meta = (BindWidget))
	// UImage* Bullet_2;
	// UPROPERTY(meta = (BindWidget))
	// UImage* Bullet_3;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BulletPanel;

	UPROPERTY(meta=(BindWidget))
	USizeBox* VisualizerSlot;

	UPROPERTY(meta=(BindWidget))
	USizeBox* VisualizerGameOverSlot;
	
	// select the target before shooting
	UPROPERTY()
	bool bSelectTarget = false;

    // UI Functions
	void IncreaseAP(const int& Amount);
	void DecreaseAP(const int& Amount);
    void UpdateAPBar();
    
    // Button Handlers
    UFUNCTION() void OnShootPressed();
    UFUNCTION() void OnShootBoostPressed();
    UFUNCTION() void OnFocusPressed();
    UFUNCTION() void OnReloadPressed();
    UFUNCTION() void OnPassPressed();

	UFUNCTION() void RefreshBulletUI();
	void ReflectBullets();
	
	UFUNCTION() void UpdateBulletIcons(const TArray<FInventoryItem>& InventoryItems);

	void ApplyBulletDisplayerFocus();
	void RevertBulletDisplayerFocus();
    
    // Targetting
    
    // Bullet Management
    UFUNCTION() void UpdateBulletSelection();
	UFUNCTION() void UpdateBulletInfo(UBulletData* BulletData);
    UFUNCTION() void UpdateTargetInfo(const FString& EnemyName, UBulletData* BulletData = nullptr); 
    UFUNCTION() void ConfirmBulletSelection();
    UFUNCTION() void SwitchToBattleUI();
	UFUNCTION()	void SetSelectedBullet(int32 Index);
	UFUNCTION() void UpdateRevolverUI();
	UFUNCTION() void HideBulletMagazineOnReload();

	/*
	 *-------------------------------------------------------
	 *  PLAYER / AI INTERACTION PLEASE DO NOT EDIT 
	 *  -----------------------------------------------------
	 */
	UPROPERTY()
	AMinigameHandler* MinigameHandler = nullptr;

	UPROPERTY()
	UBulletDisplayer* Displayer;

	UPROPERTY()
	UBulletSelector* CurrentSelectedBullet;

	UPROPERTY()
	UBulletSelector* HoveredSelectedBullet;

	// this variable will handle the target selection and will proceed to start the minigame
	UPROPERTY()
	bool bShootFired = false;

	UPROPERTY()
	bool bBulletSetupFinished = false;

	UPROPERTY()
	bool bTargetSelection = false;

	UPROPERTY()
	UICantCryGameInstance* GameInstance = nullptr;

	UPROPERTY()
	FDamage Damage;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UBulletIconWidget> BulletIconWidgetClass;

	/**
	 * Additional ap point used to boost the player's shoot
	 */
	UPROPERTY()
	int32 ApPowerBoost = 0;
	
	UFUNCTION()
	void IncreaseShootPower();

	UFUNCTION()
	void DecreaseShootPower();

	

	/*
	 *-------------------------------------------------------
	 *  PLAYER / AI INTERACTION PLEASE DO NOT EDIT 
	 *  -----------------------------------------------------
	 */


	UPROPERTY()
	AMob* CurrentActiveAI = nullptr;

	UPROPERTY()
	AMob* SelectedTarget = nullptr;

	UPROPERTY()
	AICC_Actor* SelectedActorTarget = nullptr;

	void PrepareToEngage();

	AMob* RetrieveSelectedTarget();
	void PrepareToEngageEv(const EBuffStatus& BuffToReceive);
	void PrepareToEngageEv(const EAfflictedStatus& StatusToInflict);
	void PrepareToEngageEv(const EDebuffStatus& StatusToDebuff);

	/**
	 *-----------------------------
	 *          BATTLE
	 * ----------------------------
	 */

	UPROPERTY()
	bool bStartFight = false;

	UPROPERTY()
	UVictoryVisualizer* VictoryVisualizer = nullptr;

	UPROPERTY()
	UGameOverVisualizer* GameOverVisualizer = nullptr;
	
	/**
	 * -----------------------------------
	 *         END BATTLE
	 * ----------------------------------
	 */
	
};
