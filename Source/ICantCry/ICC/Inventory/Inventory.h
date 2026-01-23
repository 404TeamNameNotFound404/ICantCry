// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/InventoryItem.h"
#include "../Actors/Bullet/EmptyCasing.h"
#include "../Actors/Bullet/GoldEmptyCasing.h"
#include "../Actors/Bullet/Essence.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Pickups/GoldEmptyCasingPickup.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Actors/Bullet/BulletData.h"

#include "Inventory.generated.h"


class UCraftingHUD;

DECLARE_MULTICAST_DELEGATE(FOnBulletCrafted)

USTRUCT(BlueprintType)
struct ICANTCRY_API FInventory
{

	GENERATED_BODY()


public:
    /**
     * Total items stored in inventory
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> Items;


    // Blueprint posseduti dal giocatore
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ERecipeType> OwnedBlueprints;

    // // Bossoli base
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // FEmptyCasing EmptyCasing;

    // // Bossoli rinforzati (gold)
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // FGoldEmptyCasing GoldCasing;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EmptyCasingCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldCasingCount;

    // Lista delle essenze raccolte
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEssence> Essences;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<FGoldEmptyCasing> GoldCasings;

    // UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    // TArray<FEmptyCasing> EmptyCasings;

    UPROPERTY()
    TMap<FString, FCasing> CasingsStored;

    UPROPERTY()
    TMap<EEssenceType, int32> EssencesInInventory;

    UPROPERTY()
    TMap<FString, FEssence> EssencesStored;

    UPROPERTY()
    TMap<ECasingType, FCasing> CasingsInInventory;

    UPROPERTY()
    TArray<FBullet> Bullets;

    UPROPERTY()
    TMap<TEnumAsByte<EBulletType>, FBullet> BulletsStored;
    
    /**
     * Total Recipes owned 
     */
    UPROPERTY()
    TArray<FRecipe> Recipes;

    UPROPERTY()
    TMap<int32, FRecipe> RecipeLists;

 
    void AddToInventory(EItemType ItemType, FBullet Bullet, const TArray<FEssence>& ItemEssences, FRecipe Recipe, int32 Quantity);

    void AddEssenceInMap(const EEssenceType& EssenceType, const int32& Quantity);

    void RemoveEssence(EEssenceType EssenceType, int32 Quantity);
	
    void RemoveFromInventory(EItemType ItemType, FBullet Bullet,  const TArray<FEssence>& ItemEssences, FRecipe Recipe, int32 Quantity = 1);

    void RemoveCasing(ECasingType CasingType, int32 Quantity);

    void RemoveBullet(UBulletData* BulletData, int32 Amount); // for battleHUD;

    bool CompareItemData(const FInventoryItem& Item, const FBullet& Bullet, const TArray<FEssence>& ItemEssences, const FRecipe& Recipe) const;

    void StarterPack();

    void AddCraftedBullet(FBullet &Bullet);

	int32 GetEssenceQuantity(EEssenceType EssenceType) const;


    // for hud
    const TArray<FInventoryItem>& GetAllItems() const;


    void SetSelectedRecipe(const FRecipe& SelectedRecipe);

    FRecipe& GetSelectedRecipe();

	FOnBulletCrafted OnBulletCrafted;

private:
    
    UPROPERTY()
    FRecipe CurrentRecipe;
};
