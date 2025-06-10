// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/InventoryItem.h"
#include "../Actors/Bullet/EmptyCasing.h"
#include "../Actors/Bullet/GoldEmptyCasing.h"
#include "../Actors/Bullet/Essence.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "Inventory.generated.h"


USTRUCT(BlueprintType)
struct ICANTCRY_API FInventory
{

	GENERATED_BODY()


public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> Items;


    // Blueprint posseduti dal giocatore
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ERecipeType> OwnedBlueprints;

    // Bossoli base
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FEmptyCasing EmptyCasing;

    // Bossoli rinforzati (gold)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGoldEmptyCasing GoldCasing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EmptyCasingCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldCasingCount;

    // Lista delle essenze raccolte
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEssence> Essences;
    /**
     * Total Recipes owned 
     */
    UPROPERTY()
    TArray<FRecipe> Recipes;

    

    bool HasBlueprint(ERecipeType RecipeType) const
    {
        return OwnedBlueprints.Contains(RecipeType);
    }
 
    void AddBlueprint(ERecipeType BlueprintType)
    {
        if (!OwnedBlueprints.Contains(BlueprintType))
        {
            OwnedBlueprints.Add(BlueprintType);
        }
    }

    void AddToInventory(EItemType ItemType, FBullet Bullet, const TArray<FEssence>& ItemEssences, FRecipe Recipe, int32 Quantity)
    {
        bool bWasAdded = false;
    
        for (FInventoryItem& Item : Items)
        {
            if (Item.ItemType == ItemType && CompareItemData(Item, Bullet, ItemEssences, Recipe))
            {
                Item.Quantity += Quantity;
                bWasAdded = true;
                break;
            }
        }

        if (!bWasAdded)
        {
            FInventoryItem NewInventoryItem;
            NewInventoryItem.ItemType = ItemType;
            NewInventoryItem.Bullet = Bullet;
            NewInventoryItem.Essences = ItemEssences;  
            NewInventoryItem.Recipe = Recipe;
            NewInventoryItem.Quantity = Quantity;
            Items.Add(NewInventoryItem);
        }

    }

    void AddCasing(ECasingType CasingType, int32 Quantity)
    {
        switch (CasingType)
        {
        case ECasingType::Base:
            EmptyCasingCount += Quantity;
            break;
        case ECasingType::Gold:
            GoldCasingCount += Quantity;
            break;
        }
    }

    void AddEssence(EEssenceType EssenceType, int32 Quantity)
    {
        DebugHelper::LogWarning("inventory -> AddEssence chiamato");

        FEssence FoundEssence;

        for (FEssence &Essence : Essences)
        {
            if (Essence.EssenceType == EssenceType)
            {
                DebugHelper::LogWarning("aggiunto una quantità " + FString::FromInt(Essence.Quantity));
                Essence.Quantity += Quantity;
                FoundEssence = Essence; // Store reference to the modified essence
                break;                  // Exit loop once found
            }
        }

        // If essence was found, add a modified copy of it

        Essences.Add(FoundEssence);
        DebugHelper::LogError("Essence size " + FString::FromInt(Essences.Num()));

        if(!FoundEssence.IsValid())
        {
            FEssence NewEssence;
            NewEssence.EssenceType = EssenceType;
            NewEssence.Quantity = Quantity;
            Essences.Add(NewEssence);
        }
        
    }

    void RemoveEssence(EEssenceType EssenceType, int32 Quantity)
    {
        for (int32 i = 0; i < Essences.Num(); ++i)
        {
            if (Essences[i].EssenceType == EssenceType)
            {
                Essences[i].Quantity = FMath::Max(0, Essences[i].Quantity - Quantity);
            
                if (Essences[i].Quantity == 0)
                {
                Essences.RemoveAt(i);
                }

                return;
            }
        }
    }
	
    void RemoveFromInventory(EItemType ItemType, FBullet Bullet,  const TArray<FEssence>& ItemEssences, FRecipe Recipe, int32 Quantity = 1)
    {
        for (int32 i = 0; i < Items.Num(); i++)
        {
            if (Items[i].ItemType == ItemType && CompareItemData(Items[i], Bullet, ItemEssences, Recipe))
            {
                Items[i].Quantity -= Quantity;
                if (Items[i].Quantity <= 0)
                {
                    Items.RemoveAt(i);
                }

                return;
            }   
        }
    }

    void RemoveCasing(ECasingType CasingType, int32 Quantity)
    {
        switch (CasingType)
        {
        case ECasingType::Base:
            EmptyCasingCount = FMath::Max(0, EmptyCasingCount - Quantity);
            break;
        case ECasingType::Gold:
            GoldCasingCount = FMath::Max(0, GoldCasingCount - Quantity);
            break;
        }

    }

    void RemoveBullet(UBulletData* BulletData, int32 Amount) // for battleHUD
    {
        for (int32 i = 0; i < Items.Num(); i++)
        {
            if (Items[i].Bullet.GetBulletData() && Items[i].Bullet.GetBulletData()->ID == BulletData->ID)
            {
                Items[i].Quantity = FMath::Max(0, Items[i].Quantity - Amount);

                if (Items[i].Quantity == 0)
                {
                    Items.RemoveAt(i);
                }

                return;
            
            }   
        }
    }

    bool CompareItemData(const FInventoryItem& Item, const FBullet& Bullet, const TArray<FEssence>& ItemEssences, const FRecipe& Recipe) const
    {
        if (!(Item.Bullet == Bullet) || !(Item.Recipe == Recipe))
        {
            return false;
        } 

        if (Item.Essences.Num() != Essences.Num())
        {
            return false;
        }
            
        TArray<FEssence> A = Item.Essences;
        TArray<FEssence> B = Essences;

        A.Sort([](const FEssence& X, const FEssence& Y) { return (uint8)X.EssenceType < (uint8)Y.EssenceType; });
        B.Sort([](const FEssence& X, const FEssence& Y) { return (uint8)X.EssenceType < (uint8)Y.EssenceType; });

        for (int32 i = 0; i < A.Num(); ++i)
        {
            if (!(A[i] == B[i]))
            return false;
        }   

        return true;
    }

	int32 GetBulletQuantity(const FBullet& Bullet) const
    {
        for (const FInventoryItem& Item : Items)
        {
            if (Item.Bullet == Bullet)  // usa operator== definito in FBullet
            {
                return Item.Quantity;
            }
        }

        return 0;
    }

	int32 GetEssenceQuantity(EEssenceType EssenceType) const
    {
        for (const FEssence& Essence : Essences)
        {
            if (Essence.EssenceType == EssenceType)
            {
                return Essence.Quantity;
            }
        }
        return 0;
    }

    int32 GetRecipeQuantity(ERecipeType RecipeType) const
    {
        int32 Count = 0;
        for (const FInventoryItem& Item : Items)
        {
            if (Item.ItemType == EItemType::Recipe && Item.Recipe.RequiredBlueprintType == RecipeType)
            {
                Count += Item.Quantity;
            }
        }
        return Count;
    }


    // for hud
    const TArray<FInventoryItem>& GetAllItems() const
    {
        return Items;
    }
};
