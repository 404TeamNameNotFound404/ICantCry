// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Inventory/Inventory.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
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
    FInventory& GetInventory();

	FInventory GetImmutableInventory() const;

    // Aggiunta/Rimozione generica
    void AddItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity = 1);
    void RemoveItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity = 1);

    // Essence
	void AddEssence(const FEssence& Essence);
	void StoreEssenceInMap(const FEssence& Essence, const int32& NewQuantity = 1);
	
 
	// Recipe
	void AddRecipe(const FRecipe& RecipeToAdd, int32 Quantity = 1);


private:

	UPROPERTY()
    FInventory Inventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UInventoryHUD* LinkedHUD;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UCraftingHUD* CraftingLinkedHUD;

	UPROPERTY()
	UICantCryGameInstance* Instance;


	UPROPERTY()
	int32 AngerCounter;

	UPROPERTY()
	int32 FearCounter;

	UPROPERTY()
	int32 DisgustCounter;

	UPROPERTY()
	int32 SadnessCounter;

	
	UPROPERTY()
	int32 JoyCounter;

	UPROPERTY()
	int32 AnxietyCounter;

	UPROPERTY()
	int32 CalmCounter;

	UPROPERTY()
	int32 JealousyCounter;
	
	UPROPERTY()
	int32 ShameCounter;
	
	UPROPERTY()
	int32 IndifferenceCounter;

    void RefreshHUD();
	
};
