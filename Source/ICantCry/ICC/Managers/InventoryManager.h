// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Inventory/Inventory.h"
#include "InventoryManager.generated.h"


class UInventoryHUD;
class UCraftingHUD;


UCLASS(Blueprintable)
class ICANTCRY_API UInventoryManager : public UObject
{
	GENERATED_BODY()

	public:

    void Initialize(UInventoryHUD* InHUD);
    void LinkCraftingHUD(UCraftingHUD* InCraftingHUD);

    // Accesso all'inventario
    FInventory GetInventory();

    // Aggiunta/Rimozione generica
    void AddItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity = 1);
    void RemoveItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity = 1);

    // Casing
    void AddCasing(ECasingType CasingType, int32 Quantity);
    void RemoveCasing(ECasingType CasingType, int32 Quantity);

    // Essence
    void AddEssence(EEssenceType EssenceType, int32 Quantity);
    void RemoveEssence(EEssenceType EssenceType, int32 Quantity);

	// Recipe
	void AddRecipe(ERecipeType RecipeType, int32 Quantity = 1);
	void AddRecipe(const FRecipe& RecipeToAdd, int32 Quantity = 1);
	void RemoveRecipe(ERecipeType RecipeType, int32 Quantity = 1);

    // Bullet diretto (es. da BattleHUD)
    void RemoveBullet(UBulletData* BulletData, int32 Quantity);

private:

	UPROPERTY()
    FInventory Inventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UInventoryHUD* LinkedHUD;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UCraftingHUD* CraftingLinkedHUD;

    void RefreshHUD();
	
};
