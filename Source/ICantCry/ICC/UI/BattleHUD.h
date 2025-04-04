// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "../Mechanics/TurnSystem/Core/BattleHandler.h"
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
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasMiniGames; 
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasAmmoSelection;
    UPROPERTY(meta = (BindWidget)) UWidget* CanvasStatus;

    // Action Buttons
    UPROPERTY(meta = (BindWidget)) UButton* Shoot;
    UPROPERTY(meta = (BindWidget)) UButton* ShootBoost;
    UPROPERTY(meta = (BindWidget)) UButton* Focus;
    UPROPERTY(meta = (BindWidget)) UButton* Reload;
    UPROPERTY(meta = (BindWidget)) UButton* Pass;
    UPROPERTY(meta = (BindWidget)) UButton* ConfirmButton;

    // Status Bars
    UPROPERTY(meta = (BindWidget)) UProgressBar* PlayerHealth;
    UPROPERTY(meta = (BindWidget)) UProgressBar* APBar;

    // Ammo Display
    UPROPERTY(meta = (BindWidget)) UImage* Magazine;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_1;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_2;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_3;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_4;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_5;
    UPROPERTY(meta = (BindWidget)) UImage* Ammo_6;

    // Targetting
    UPROPERTY(meta = (BindWidget)) UImage* Crosshair;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText; 

    // Bullet Selection
    UPROPERTY(meta = (BindWidget)) UImage* AmmoSelectionIndicator;
    UPROPERTY(meta = (BindWidget)) TArray<UImage*> BulletIcons;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetText_2; // Bullet name
    UPROPERTY(meta = (BindWidget)) UTextBlock* TargetNameText_2; // Bullet name
 
    // Bullet Data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
    TArray<TSubclassOf<UBulletData>> AvailableBulletTypes;


    UFUNCTION(BlueprintCallable)void ScrollBulletSelection(int ScrollValue);

private:

    UPROPERTY() ABattleHandler*  BattleHandler = nullptr;


    // Game State
    UPROPERTY()
    int CurrentAP = 0;

    int32 CurrentEnemyIndex = 0;
    int32 SelectedBulletIndex = 0;
    int32 CurrentRevolverSlot = 0;
    UPROPERTY(EditDefaultsOnly, Category = "Bullets") int32 MaxRevolverSlots = 6;

    TArray<AActor*> Enemies;
    TArray<UImage*> RevolverSlots;
    TArray<UBulletData*> LoadedBulletData;
    UPROPERTY() UBulletData* CurrentBulletData;

    // UI Functions
    UFUNCTION() void IncreaseAP(int Amount);
    UFUNCTION() void UpdateAPBar();
    
    // Button Handlers
    UFUNCTION() void OnShootPressed();
    UFUNCTION() void OnShootBoostPressed();
    UFUNCTION() void OnFocusPressed();
    UFUNCTION() void OnReloadPressed();
    UFUNCTION() void OnPassPressed();
    
    // Targetting
    UFUNCTION() void ScrollTargetSelection(float ScrollValue);
    UFUNCTION() void UpdateTarget();
    UFUNCTION() void UpdateCrosshair();
    
    // Bullet Management
    UFUNCTION() void UpdateBulletSelection();
	UFUNCTION() void UpdateBulletInfo(UBulletData* BulletData);
    UFUNCTION() void UpdateTargetInfo(const FString& EnemyName, UBulletData* BulletData = nullptr); 
    UFUNCTION() void ConfirmBulletSelection();

    UFUNCTION() void SwitchToBattleUI();


	
};
