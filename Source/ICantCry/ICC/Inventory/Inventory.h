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

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<FGoldEmptyCasing> GoldCasings;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TArray<FEmptyCasing> EmptyCasings;

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

    void AddEssenceInMap(const EEssenceType& EssenceType, const int32& Quantity)
    {
        if (EssencesInInventory.Contains(EssenceType))
        {
            EssencesInInventory[EssenceType] += Quantity;
        }
        else
        {
            EssencesInInventory.Add(EssenceType, Quantity);
        }
    }

    int32 GetEssenceQuantityInMap(const EEssenceType& EssenceType) const
    {
        return EssencesInInventory.Contains(EssenceType) ? EssencesInInventory[EssenceType] : 0;
    }

    void AddEssence(EEssenceType EssenceType, int32 Quantity)
    {

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

    void StarterPack()
    {
        UTexture2D* IndifferenceIcon = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/StarterContent/Textures/T_Ceramic_Tile_M.T_Ceramic_Tile_M")));
        checkf(IndifferenceIcon, TEXT("Invalid"))
        FBullet Indifference;
        UBulletData* Data = NewObject<UBulletData>();
        Data->BulletName = "Indifference";
        Data->Icon = IndifferenceIcon;
        Data->Type = EBulletType::Indifference;
        Data->Power = 5.0f;
        Data->WeaknessModifier = 0.8f;
        Data->DisplayColor = FColor::White;
        Data->Description = "Indifference starter pack";

        Indifference.SetBulletData(Data);
        Indifference.SetQuantity(6);

        UTexture2D* SadnessIcon = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/StarterContent/Textures/T_Fire_Tiled_D")));
        FBullet Sadness;
        UBulletData* SadnessData = NewObject<UBulletData>();
        SadnessData->BulletName = "Sadness";
        SadnessData->Icon = SadnessIcon;
        SadnessData->Type = EBulletType::Sadness;
        SadnessData->Power = 12.0f;
        SadnessData->DisplayColor = FColor::Red;
        SadnessData->Description = "Sadness starter pack test";

        Sadness.SetBulletData(SadnessData);
        Sadness.SetQuantity(3);

        
        BulletsStored.Add(Data->Type, Indifference);
        BulletsStored.Add(SadnessData->Type, Sadness);
    }

    void AddCraftedBullet(FBullet &Bullet)
    {
        Bullets.Add(Bullet);
        
        checkf(Bullet.GetBulletData(), TEXT("Bullet data is null during add"))

        EBulletType Key = Bullet.GetBulletData()->Type;
        
        if (BulletsStored.Contains(Key))
        {
            int32 ExistingQuantity = BulletsStored[Key].GetQuantity();
            BulletsStored[Key].SetQuantity(ExistingQuantity + 1);
            DebugHelper::LogWarning("Already registered updating it's quantity " + FString::FromInt(BulletsStored[Key].GetQuantity()));
        }
        else
        {
            FBullet NewBullet;
            NewBullet.SetBulletData(Bullet.GetBulletData());
            NewBullet.SetQuantity(1);

            BulletsStored.Add(Key, NewBullet);
            DebugHelper::LogSuccess("New bullet crafted and added: " + Bullet.GetBulletData()->BulletName);
        }
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


    void SetSelectedRecipe(const FRecipe& SelectedRecipe)
    {
        DebugHelper::LogMessage(4, FColor::Purple, "Selected Recipe Quantity " + FString::FromInt(SelectedRecipe.Requirements->EssenceQuantity) + "\nCasing Quantity " + FString::FromInt(SelectedRecipe.Requirements->CasingQuantity));
        CurrentRecipe = SelectedRecipe;
        CurrentRecipe.ResultBullet.SetBulletData(nullptr);
        CurrentRecipe.ResultBullet.SetBulletData(SelectedRecipe.ResultBullet.GetBulletData());
    }

    FRecipe& GetSelectedRecipe() 
    {
        return CurrentRecipe;
    }

private:
    
    UPROPERTY()
    FRecipe CurrentRecipe;
};
