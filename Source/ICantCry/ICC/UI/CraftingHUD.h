// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"



#include "EssenceWidget.h"
#include "../Inventory/Inventory.h"
#include "../Inventory/CraftingTable.h"
#include "../Inventory/CraftingTable.h"
#include "../Actors/Bullet/BulletData.h"
#include "../Source/ICantCry/ICC/Input/ICC_PlayerController.h"
#include "../Source/ICantCry/ICC/Managers/InventoryManager.h"

#include "Kismet/KismetSystemLibrary.h"
#include "CraftingHUD.generated.h"



class AICC_Player;

class URecipeButtonWidget;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UCraftingHUD : public UUserWidget
{
	GENERATED_BODY()

public: 
	 
	void SetCraftingTable(UCraftingTable* InTable);
	void RefreshUI(); // Pulisce tutti i widget da inventario e popola la lista delle recipe, essenze e bossoli
    void RefreshRecipesList();
	void UpdateEssenceList();
	void UpdateSelectedRecipeDetails(); //Mostra dati bullet, materiali, effetto
    void UpdateMaterialList(); //Mostra essenze richieste con quantità
    void UpdateCraftButton(); // Attiva/disattiva bottone in base alle risorse
	void OnRecipeSelected( const FRecipe& NewRecipe); // Seleziona una blueprint, aggiorna i dettagli
	void SetupCraftingHUD(UCraftingTable* InCraftingTable, UInventoryManager* InInventoryManager);

	FInventory GetInventory() const;
	
protected:

	virtual void NativeConstruct() override;

protected:

	// SX
	UPROPERTY(meta = (BindWidget)) UScrollBox* RecipeListScrollBar;

	// CENTER
	UPROPERTY(meta = (BindWidget)) UImage* BulletImage;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* RecipeDescription;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletEffectText;
    UPROPERTY(meta = (BindWidget)) UButton* CraftButton;

	// DX
    UPROPERTY(meta = (BindWidget)) UTextBlock* EmptyCasingCountText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* GoldCasingCountText;
	UPROPERTY(meta = (BindWidget)) UScrollBox* EssenceListScrollBar;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UCraftingTable* CraftingTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UInventoryManager* InventoryManager;
    
    FRecipe SelectedRecipe; 

	// Mappa icone associate ai tipi di essenza
	// UPROPERTY(EditDefaultsOnly, Category = "UI")
	// TMap<EEssenceType, UTexture2D*> EssenceIcons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<URecipeButtonWidget> RecipeButtonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UEssenceWidget> EssenceWidgetClass;

	UPROPERTY()
	URecipeButtonWidget* RecipeButtonWidget = nullptr;

	UPROPERTY()
	AICC_PlayerController* Controller;

	UPROPERTY()
	AICC_Player* Player = nullptr;

	UPROPERTY()
	FInventory Inventory;

	// AICC_PlayerController* Controller;

	UFUNCTION()
    void OnCraftClicked();

    
};
