// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingTable.h"

void UCraftingTable::Initialize(FInventory *InInventory)
{

    if (!InInventory)
    {
        UE_LOG(LogTemp, Error, TEXT("Inventory null on UCraftingTable::Initialize"));
        return;
    }

    MasterRecipes.Empty();


}

void UCraftingTable::Initialize()
{
    
    MasterRecipes.Empty();

}


void UCraftingTable::CraftBullet(const FBullet& BulletToCraft, ERecipeType BlueprintType, ECasingType CasingType)
{
    
    // check if there is a valid recipe for this bullet
    // if (!(Recipe.ResultBullet == BulletToCraft &&
    //     Recipe.RequiredBlueprintType == BlueprintType &&
    //     Recipe.RequiredCasingType == CasingType))
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("something gone wrong"));
    //     return;
    // }

   
    CheckResurces();

    if (!IsCraftable)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Not enough resources to craft bullet: %s"), *BulletToCraft.Name.ToString());
        return;
    }

    
    Craft();

    //UE_LOG(LogTemp, Log, TEXT("Crafted bullet: %s"), *BulletToCraft.Name.ToString());
}

void UCraftingTable::SetRecipe( const FRecipe& InRecipe)
{
    Recipe = InRecipe;
}

void UCraftingTable::CheckResurces()
{
    


    // Blueprint (Recipe) check
    if (!Inventory.HasBlueprint(Recipe.RequiredBlueprintType))
    {
        IsCraftable = false;
        return;
    }


    // check casing
    int32 AvailableCasing = 0;
    switch (Recipe.RequiredCasingType)
    {
    case ECasingType::Base:
        AvailableCasing =  Inventory.EmptyCasingCount;
        break;
    case ECasingType::Gold:
        AvailableCasing = Inventory.GoldCasingCount;
        break;
    default:
        IsCraftable = false;
        return;
    }

    if (AvailableCasing < Recipe.RequiredCasingQuantity)
    {
        IsCraftable = false;
        return;
    }

    // check essence
    for (const FEssence& RequiredEssence : Recipe.RequiredEssences)
    {
        int32 AvailableEssence = Inventory.GetEssenceQuantity(RequiredEssence.EssenceType);
        if (AvailableEssence < RequiredEssence.Quantity)
        {
            IsCraftable = false;
            return;
        }
    }

    
    IsCraftable = true;
}

void UCraftingTable::Craft()
{

    if (!IsCraftable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot craft: missing resources or invalid recipe."));
        return;
    }

    // consume casing
    switch (Recipe.RequiredCasingType)
    {
    case ECasingType::Base:
        Inventory.EmptyCasingCount -= Recipe.RequiredCasingQuantity;
        break;
    case ECasingType::Gold:
        Inventory.GoldCasingCount -= Recipe.RequiredCasingQuantity;
        break;
    }

    // consume essences
    for (const FEssence& RequiredEssence : Recipe.RequiredEssences)
    {
        for (FEssence& InventoryEssence : Inventory.Essences)
        {
            if (InventoryEssence.EssenceType == RequiredEssence.EssenceType)
            {
                InventoryEssence.Quantity -= RequiredEssence.Quantity;
                break;
            }
        }
    }

    // add bullet to inventory
    Inventory.AddToInventory(EItemType::Bullet, Recipe.ResultBullet,  TArray<FEssence>(), FRecipe(), 1);

    // UE_LOG(LogTemp, Log, TEXT("Successfully crafted bullet: %s"),
    //     *Recipe->ResultBullet->GetName());

    
    IsCraftable = false;
}

const TArray<FRecipe> &UCraftingTable::GetAvailableRecipes() const
{
    static TArray<FRecipe> AvailableRecipes;

    AvailableRecipes.Empty();
    
    for (const FRecipe& RecipeDef : MasterRecipes)
    {
        // Mostra solo se il player ha la blueprint corrispondente
        if (Inventory.OwnedBlueprints.Contains(RecipeDef.RequiredBlueprintType))
        {
            AvailableRecipes.Add(RecipeDef);
        }
    }

    return AvailableRecipes;

}


// void UCraftingTable::SetInventoryReference(FInventory* InventoryRef)
// {
//     PlayerInventoryRef = InventoryRef;
// }


