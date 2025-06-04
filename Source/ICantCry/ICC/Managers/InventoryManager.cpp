// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManager.h"
#include "../UI/InventoryHUD.h"
#include "../UI/CraftingHUD.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"

void UInventoryManager::Initialize(UInventoryHUD* InHUD)
{
    LinkedHUD = InHUD;

    if (LinkedHUD)
    {
       DebugHelper::LogWarning("InventoryHUD collegato correttamente!");
    }
}

void UInventoryManager::LinkCraftingHUD(UCraftingHUD *InCraftingHUD)
{
    CraftingLinkedHUD = InCraftingHUD;
}

FInventory& UInventoryManager::GetInventory()
{
    return Inventory;
}

void UInventoryManager::AddItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity)
{
    Inventory.AddToInventory(ItemType, Bullet, Essences, Recipe, Quantity);
    DebugHelper::LogWarning("AddItem chiamato correttamente!");
    RefreshHUD();
}

void UInventoryManager::RemoveItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity)
{
    Inventory.RemoveFromInventory(ItemType, Bullet, Essences, Recipe, Quantity);
    RefreshHUD();
}

void UInventoryManager::AddCasing(ECasingType CasingType, int32 Quantity)
{
    Inventory.AddCasing(CasingType, Quantity);
    RefreshHUD();
}

void UInventoryManager::RemoveCasing(ECasingType CasingType, int32 Quantity)
{
    Inventory.RemoveCasing(CasingType, Quantity);
    RefreshHUD();
}

void UInventoryManager::AddEssence(EEssenceType EssenceType, int32 Quantity)
{
    DebugHelper::LogWarning("UInventoryManager->AddEssence chiamato");
    Inventory.AddEssence(EssenceType, Quantity);
    RefreshHUD();
}

void UInventoryManager::RemoveEssence(EEssenceType EssenceType, int32 Quantity)
{
    Inventory.RemoveEssence(EssenceType, Quantity);
    RefreshHUD();
}


void UInventoryManager::AddRecipe(ERecipeType RecipeType, int32 Quantity)
{
    for (int32 i = 0; i < Quantity; ++i)
    {
        Inventory.OwnedBlueprints.Add(RecipeType);
    }

    RefreshHUD();
}

void UInventoryManager::RemoveRecipe(ERecipeType RecipeType, int32 Quantity)
{
    int32 Count = 0;

    for (int32 i = Inventory.OwnedBlueprints.Num() - 1; i >= 0 && Count < Quantity; --i)
    {
        if (Inventory.OwnedBlueprints[i] == RecipeType)
        {
            Inventory.OwnedBlueprints.RemoveAt(i);
            ++Count;
        }
    }

    RefreshHUD();
}

void UInventoryManager::RemoveBullet(UBulletData* BulletData, int32 Quantity)
{
    Inventory.RemoveBullet(BulletData, Quantity);
    RefreshHUD();
}

void UInventoryManager::RefreshHUD()
{
    DebugHelper::LogWarning("RefreshHUD Chiamato");


    check(LinkedHUD)
    check(CraftingLinkedHUD)

    UE_LOG(LogTemp, Warning, TEXT("LinkedHUD: %s"), LinkedHUD ? TEXT("VALID") : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("CraftingLinkedHUD: %s"), CraftingLinkedHUD ? TEXT("VALID") : TEXT("NULL"));

    if (LinkedHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("LinkedHUD->IsInViewport(): %s"), LinkedHUD->IsInViewport() ? TEXT("YES") : TEXT("NO"));
    }

    if (CraftingLinkedHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("CraftingLinkedHUD->IsInViewport(): %s"), CraftingLinkedHUD->IsInViewport() ? TEXT("YES") : TEXT("NO"));
    }

    

    if (LinkedHUD ) //&& LinkedHUD->IsInViewport()
    {
        DebugHelper::LogWarning("LinkedHUD Chiamato");   
        LinkedHUD->UpdateInventoryDisplay(Inventory);   
    }

    if (CraftingLinkedHUD ) //&& CraftingLinkedHUD->IsInViewport()
    {
        DebugHelper::LogWarning("CraftingLinkedHUD Chiamato");
        CraftingLinkedHUD->RefreshUI(); 
    }
}