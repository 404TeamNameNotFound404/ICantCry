// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Inventory/Inventory.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "BulletBottonItem.h"

#include "../Source/ICantCry/ICC/Input/ICC_PlayerController.h"
#include "InventoryHUD.generated.h"

class AICC_Player;
/**
 * 
 */
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

protected:

    //left pannel
    UPROPERTY(meta = (BindWidget)) UScrollBox* BulletListContainer; // Lista proiettili (sinistra)
    
	// Right pannel
	UPROPERTY(meta = (BindWidget)) UImage* SelectedBulletImage;  // icon proiettile (pannello di dx)
	UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletPower;
    // UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletEffectiveness;
    // UPROPERTY(meta = (BindWidget)) UTextBlock* SelectedBulletWeakness;

	// Template per i bottoni dei proiettili
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UBulletBottonItem> BulletButtonClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
    FLinearColor SelectedColor;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    FLinearColor UnselectedColor;


private:

 	void ClearBulletList();
    void PopulateBulletList(const TArray<FInventoryItem>& Items);
    void UpdateDetailPanel(const FBullet& Bullet);
    
	void MoveSelectionUp();
	void MoveSelectionDown();

    TArray<FBullet> DisplayedBullets;
    int32 CurrentSelectedIndex;
    TArray<UBulletBottonItem*> BulletButtons;
    
    
	
};
