// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManager.h"
#include "../UI/InventoryHUD.h"
#include "../UI/CraftingHUD.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"

#include "Engine/GameInstance.h"

void UInventoryManager::Initialize(UInventoryHUD* InHUD)
{
    LinkedHUD = InHUD;

    Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    checkf(Instance, TEXT("Instance is null"))

    Instance->SetInventory(Inventory);

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

FInventory UInventoryManager::GetImmutableInventory() const
{
    return Inventory;
}

void UInventoryManager::AddItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity)
{
    Inventory.AddToInventory(ItemType, Bullet, Essences, Recipe, Quantity);
    RefreshHUD();
}

void UInventoryManager::RemoveItem(EItemType ItemType, const FBullet& Bullet, const TArray<FEssence>& Essences, const FRecipe& Recipe, int32 Quantity)
{
    Inventory.RemoveFromInventory(ItemType, Bullet, Essences, Recipe, Quantity);
    RefreshHUD();
}

void UInventoryManager::AddCasing(ECasingType CasingType, int32 Quantity)
{
    //Inventory.AddCasing();
   // RefreshHUD();
    
}

void UInventoryManager::RemoveCasing(ECasingType CasingType, int32 Quantity)
{
    Inventory.RemoveCasing(CasingType, Quantity);
    RefreshHUD();
}

void UInventoryManager::AddEssence(EEssenceType EssenceType, int32 Quantity)
{
    
}

void UInventoryManager::AddEssence2(const FEssence& Essence)
{
    Instance->GetInventory().AddEssenceInMap(Essence.EssenceType, Essence.Quantity);
    Instance->GetInventory().Essences.Add(Essence);

    StoreEssenceInMap(Essence, Essence.Quantity);
    
    switch (Essence.EssenceType)
    {
    default:
        break;
    case EEssenceType::Indifference:
        IndifferenceCounter++; // real quantity added
        break;
    case EEssenceType::Anger:
        AngerCounter++;
        break;
    case EEssenceType::Fear:
        FearCounter++;
        break;
    case EEssenceType::Disgust:
        DisgustCounter++;
        break;
    case EEssenceType::Sadness:
        SadnessCounter++;
        break;
    case EEssenceType::Joy:
        JoyCounter++;
        break;
    case EEssenceType::Anxiety:
        AnxietyCounter++;
        break;
    case EEssenceType::Calm:
        CalmCounter++;
        break;
    case EEssenceType::Jealousy:
        JealousyCounter++;
        break;
    case EEssenceType::Shame:
        ShameCounter++;
        //DebugHelper::LogSuccess("Added indifference -> size " + FString::FromInt(ShameCounter));
        break;
    }
}

void UInventoryManager::StoreEssenceInMap(const FEssence& Essence, const int32& NewQuantity)
{
    const FString Name = Essence.GetName(Essence.EssenceType);

    if (Instance->GetInventory().EssencesStored.Contains(Name))
    {
        const int32 ExistingQuantity = Instance->GetInventory().EssencesStored[Name].Quantity;
        Instance->GetInventory().EssencesStored[Name].Quantity = ExistingQuantity + NewQuantity;
    }

    else
    {
        Instance->GetInventory().EssencesStored.Add(Name, Essence);
    }
}

int32 UInventoryManager::GetEssenceQuantityForType(const EEssenceType& EssenceType)
{
    switch (EssenceType)
    {
    case EEssenceType::Indifference:
        return IndifferenceCounter;
    case EEssenceType::Anger:
        return AngerCounter;
    case EEssenceType::Fear:
        return FearCounter;
    case EEssenceType::Disgust:
        return DisgustCounter;
    case EEssenceType::Sadness:
        return SadnessCounter;
    case EEssenceType::Joy:
        return JoyCounter;
    case EEssenceType::Anxiety:
        return AnxietyCounter;
    case EEssenceType::Calm:
        return CalmCounter;
    case EEssenceType::Jealousy:
        return JealousyCounter;
    case EEssenceType::Shame:
       // DebugHelper::LogSuccess("GetEssenceQuantityForShame");
        return ShameCounter;
    }

    return 0;
}

void UInventoryManager::AddEssence(const FEssence& Essence, const int32& Quantity)
{
    Instance->GetInventory().Essences.Add(Essence);
}

void UInventoryManager::RemoveEssence(EEssenceType EssenceType, int32 Quantity)
{
    Inventory.RemoveEssence(EssenceType, Quantity);
    RefreshHUD();
}


void UInventoryManager::AddRecipe(ERecipeType RecipeType, int32 Quantity)
{
    Inventory.OwnedBlueprints.Add(RecipeType);
  //  RefreshHUD();
}



void UInventoryManager::AddRecipe(const FRecipe& RecipeToAdd, int32 Quantity)
{
    Instance->GetInventory().Recipes.Add(RecipeToAdd);
    Instance->GetInventory().RecipeLists.Add(RecipeToAdd.Index, RecipeToAdd);
    Instance->GetInventory().OwnedBlueprints.Add(RecipeToAdd.RequiredBlueprintType);
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

    //RefreshHUD();
}

void UInventoryManager::RemoveBullet(UBulletData* BulletData, int32 Quantity)
{
    Inventory.RemoveBullet(BulletData, Quantity);
    RefreshHUD();
}

void UInventoryManager::RefreshHUD()
{
    
    check(LinkedHUD)
    check(CraftingLinkedHUD)
    

    if (LinkedHUD ) //&& LinkedHUD->IsInViewport()
    {
        LinkedHUD->UpdateInventoryDisplay(Inventory);   
    }

    if (CraftingLinkedHUD ) //&& CraftingLinkedHUD->IsInViewport()
    {
        CraftingLinkedHUD->RefreshUI(); 
    }
}