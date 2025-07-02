// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "../Mechanics/TurnSystem/Core/BattleHandler.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/CircularBulletBuffer.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/RevolverSlot.h"
#include "../Inventory/Inventory.h"
#include "Blueprint/UserWidget.h"
#include "../Inventory/BulletIconWidget.h"
#include "ICantCry/ICC/Mechanics/UI/BulletDisplay/BulletDisplayer.h"
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
	UPROPERTY(meta = (BindWidget)) UImage* AmmoSelectionIndicator;
    UPROPERTY(meta = (BindWidget)) UCanvasPanel* CanvasStatus;
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* CanvasBulletStats;


    // Action Buttons
    UPROPERTY(meta = (BindWidget)) UButton* Shoot;
    UPROPERTY(meta = (BindWidget)) UButton* ShootBoost;
    UPROPERTY(meta = (BindWidget)) UButton* Focus;
    UPROPERTY(meta = (BindWidget)) UButton* Reload;
    UPROPERTY(meta = (BindWidget)) UButton* Pass;
    UPROPERTY(meta = (BindWidget)) UButton* ConfirmButton;
	UPROPERTY(meta = (BindWidget)) UButton* EngageBtn;

    // Status Bars
    UPROPERTY(meta = (BindWidget)) UProgressBar* PlayerHealth;
    UPROPERTY(meta = (BindWidget)) UProgressBar* APBar;

    // Ammo Display // Revolver magazine (CanvasFirstReloadMagazine)
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

	UPROPERTY() TArray<UImage*> PistolMagazines;
	
    TArray<UBulletData*> LoadedBulletData;
	UBulletDisplayer* GetBulletDisplayer() const;
    
    UPROPERTY(EditDefaultsOnly, Category = "Bullets") int32 MaxRevolverSlots = 6;
	int32 SelectedBulletIndex = 0;
    int32 CurrentRevolverSlot = 0;

    // Targetting
    UPROPERTY(meta = (BindWidget)) UImage* Crosshair;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StatusText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_1;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText_3;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_3;

    // Bullet Selection
    //UPROPERTY(meta = (BindWidget)) TArray<UImage*> BulletIcons;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText_2; // Bullet name
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_2; // Bullet name
	UPROPERTY(meta = (BindWidget)) UTextBlock* BulletName; // canvasbulletstats
	UPROPERTY(meta = (BindWidget)) UTextBlock* Quantity; // è il titolo 
	UPROPERTY(meta = (BindWidget)) UTextBlock* QuantityTotal; //  è il text dove verrà scritto il numero
	UPROPERTY(meta = (BindWidget)) UTextBlock* Description;

	UFUNCTION(BlueprintCallable) void ScrollBulletSelection(float ScrollValue);

	// bullet images (in the CanvasAmmoSelection)
	UPROPERTY(meta = (BindWidget))
    UHorizontalBox* BulletPanel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
	TSubclassOf<class UBulletIconWidget> BulletIconWidgetClass;
	

    // Bullet Data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
    TArray<TSubclassOf<UBulletData>> AvailableBulletTypes;


	// ANIMATION 
	UPROPERTY(EditDefaultsOnly, Category="UI Settings")
    float SelectionAnimSpeed = 8.f;
    
    UPROPERTY(EditDefaultsOnly, Category="UI Settings")
    bool bAnimateSelection = true;

	UPROPERTY()
	UBulletSelector* CurrentSelectedBullet;

    
	void UpdateTarget();
	void ScrollTargetSelection(float ScrollValue);

	void SetSelectTarget(const bool& Enable);
	bool GetSelectTarget() const;
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

private:

    UPROPERTY() ABattleHandler*  BattleHandler = nullptr;
	UPROPERTY() FInventory Inventory;
	UPROPERTY() UBulletData* CurrentBulletData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category="Bullets")
	TSubclassOf<UBulletDisplayer> BulletDisplayerClass;


	// Circular Buffer
	UPROPERTY()
    UCircularBulletBuffer* RevolverBuffer; // represents the bullets already loaded into the revolver

	
    // Game State
    UPROPERTY()
    int CurrentAP = 0;

    int32 CurrentEnemyIndex = 0;
	int32 BulletQuantity;
    

    TArray<AActor*> Enemies; // non ha senso sta variabile qua , abbiamo la queue sul battle handler
    
	
	// select the target before shooting
	UPROPERTY()
	bool bSelectTarget = false;

    // UI Functions
    UFUNCTION() void IncreaseAP(int Amount);
    UFUNCTION() void UpdateAPBar();
    
    // Button Handlers
    UFUNCTION() void OnShootPressed();
    UFUNCTION() void OnShootBoostPressed();
    UFUNCTION() void OnFocusPressed();
    UFUNCTION() void OnReloadPressed();
    UFUNCTION() void OnPassPressed();
    

    
    // Bullet Management
    UFUNCTION() void UpdateBulletSelection();
	UFUNCTION() void UpdateBulletInfo(UBulletData* BulletData); // Aggiorna le informazioni testuali del proiettile
	UFUNCTION() void UpdateBulletStats(UBulletData* BulletData);   // Aggiorna le statistiche del proiettile
	UFUNCTION() void UpdateBulletIcons(const TArray<FInventoryItem>& InventoryItems); // Aggiorna le icone dei proiettili disponibili
    UFUNCTION() void UpdateTargetInfo(const FString& EnemyName, UBulletData* BulletData = nullptr); 
	UFUNCTION() void RefreshBulletUI(); // Aggiorna l'intera UI dei proiettili
    UFUNCTION()	void SetSelectedBullet(int32 Index);
	UFUNCTION() void UpdateRevolverUI(); // Aggiorna l'UI del revolver
    UFUNCTION() void ConfirmBulletSelection(); // Conferma la selezione del proiettile
    UFUNCTION() void SwitchToBattleUI();
	UFUNCTION() void SwitchToBattlePhase();

	UFUNCTION()
    void CleanRef();
	

	/*
	 *-------------------------------------------------------
	 *  PLAYER / AI INTERACTION PLEASE DO NOT EDIT 
	 *  -----------------------------------------------------
	 */
	UPROPERTY()
	AMinigameHandler* MinigameHandler = nullptr;

	// this variable will handle the target selection and will proceed to start the minigame
	UPROPERTY()
	bool bShootFired = false;

	UPROPERTY()
	bool bBulletSetupFinished = false;

	UPROPERTY()
	bool bTargetSelection = false;

	UPROPERTY()
	FDamage Damage;

	UPROPERTY()
	UBulletDisplayer* Displayer;
	
	void ReflectBullets();

	/*
	 *-------------------------------------------------------
	 *  PLAYER / AI INTERACTION PLEASE DO NOT EDIT 
	 *  -----------------------------------------------------
	 */

	/**
	 *-----------------------------
	 *          BATTLE
	 * ----------------------------
	 */

	UPROPERTY()
	bool bStartFight = false;
	
	/**
	 * -----------------------------------
	 *         END BATTLE
	 * ----------------------------------
	 */
	
};
