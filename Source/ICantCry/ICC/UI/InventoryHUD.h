// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Inventory/Inventory.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "BulletBottonItem.h"
#include "CasingWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "../UI/EssenceWidget.h" 
#include "ICantCry/ICC/Inventory/CraftingTable.h"
#include "Inventory/StandardBulletDisplayer.h"
#include "Inventory/GoldBulletDisplayer.h"
#include "Components/ProgressBar.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "InventoryHUD.generated.h"

class AICC_Player;





UCLASS()
class ICANTCRY_API UInventoryHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	// Chiamato quando il widget viene creato
    void NativeConstruct() override;

	// Aggiorna tutta la UI con i dati dell'inventario corrente
    UFUNCTION(BlueprintCallable)
    void UpdateInventoryDisplay(const FInventory& Inventory);

    void SelectBullet(int32 Index);

	void Refresh();

	void Setup();



public: 

	// LEFT PANEL
	UPROPERTY(meta = (BindWidget))  UWidgetSwitcher* BulletSwitcher; // page switcher for bullets
	UPROPERTY(meta = (BindWidget))  UButton* ButtonSwitcher;

	// contentswitcher pages
    UPROPERTY(meta = (BindWidget))  UWidget* StandardBulletPage;
	UPROPERTY(meta = (BindWidget)) UWidget* GoldBulletPage;
    

	// MID
	UPROPERTY(meta = (BindWidget)) UImage* SelectedBulletImage;  // icon proiettile (pannello centrale)
	UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletPower;
    UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletEffectiveness;
    UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletWeakness;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CraftInfo;
	UPROPERTY(meta = (BindWidget))  UButton* CraftButton;
	UPROPERTY(meta = (BindWidget))  UProgressBar* CraftingProgressBar;
	UPROPERTY(meta = (BindWidget)) UTextBlock* BlueprintRequirementTxt;
	
	
	// RIGHT PANEL
	UPROPERTY(meta = (BindWidget))  UScrollBox* EssenceBox;

	UPROPERTY(meta = (BindWidget)) UStandardBulletDisplayer* StandardBulletDisplayer;

	UCraftingTable* GetTable();

protected:

	// Template per i bottoni dei proiettili
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UBulletBottonItem> BulletButtonClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
    FLinearColor SelectedColor;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    FLinearColor UnselectedColor;


private:

	void RefreshEssence();
 	void ClearBulletList();

	
    void UpdateDetailPanel(const FBullet& Bullet);
    
	void MoveSelectionUp();
	void MoveSelectionDown();

	// CRAFT
	UFUNCTION() void OnCraftClicked();
	UFUNCTION(BlueprintPure) FText OnQuantityChanged();
	UFUNCTION(BlueprintPure) FText OnGoldQuantityChanged();

	UFUNCTION() void OnToggleSwitcher();

	UFUNCTION() void UpdateCraft();

	UPROPERTY() bool bIsHolding = false;

	UFUNCTION() void OnCraftReleased();
    
	UPROPERTY() float CurrentProgress;

	UPROPERTY() FTimerHandle Timer;




	UPROPERTY()
    TArray<FBullet> DisplayedBullets;

	UPROPERTY()
    int32 CurrentSelectedIndex;

	UPROPERTY()
	UICantCryGameInstance* GameInstance;

	UPROPERTY()
	FInventory ImmutableInventory;

	UPROPERTY()
	TMap<FString, UCasingWidget*> CasingWidgetsStored;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI-Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCasingWidget> CasingWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI-Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEssenceWidget> UEssenceWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI-Class", meta = (AllowPrivateAccess = "true"))
	float ProgressBarSpeed = 1.0f;

	UPROPERTY()
	UCraftingTable* CraftingTable;
};
