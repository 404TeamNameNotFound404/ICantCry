// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingTable.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/UI/CraftingHUD.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UCraftingTable::Initialize(FInventory *InInventory, UWorld* World)
{

    if (!InInventory)
    {
        UE_LOG(LogTemp, Error, TEXT("Inventory null on UCraftingTable::Initialize"));
        return;
    }

    MasterRecipes.Empty();
    
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(World));
    checkf(Instance, TEXT("Instance invalid"))
    Inventory = Instance->GetInventory();

    AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

    Player = Cast<AICC_Player>(Controller->GetPawn());
    checkf(Player, TEXT("Player is invalid"))
}

void UCraftingTable::Initialize(UWorld* World)
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(World));
    checkf(Instance, TEXT("Instance invalid"))
    Inventory = Instance->GetInventory();
    MasterRecipes.Empty();

    // AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
    //
    // Player = Cast<AICC_Player>(Controller->GetPawn());
    // checkf(Player, TEXT("Player is invalid"))

    DebugHelper::LogMessage(5, FColor::Black, "UCraftingTable::Initialize called");
}


void UCraftingTable::CraftBullet(const FBullet& BulletToCraft, ERecipeType BlueprintType, ECasingType CasingType)
{
    ScanResources();

    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();
    FBullet Bullet = SelectedRecipe.ResultBullet;
    Instance->GetInventory().AddCraftedBullet(Bullet);
    
    Craft();
}

void UCraftingTable::CraftSelectedBullet(UWorld* World)
{
    ScanResources(World);
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();

    if (!SelectedRecipe.ResultBullet.GetBulletData())
    {
        DebugHelper::LogError("Bullet data is null can't craft");
        return;
    }
    
    FBullet Bullet = SelectedRecipe.ResultBullet;
    Instance->GetInventory().AddCraftedBullet(Bullet);

    Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    Craft(World);
}

void UCraftingTable::SetRecipe( const FRecipe& InRecipe)
{
    Recipe = InRecipe;
}

bool UCraftingTable::ScanResources()
{
    DebugHelper::LogSuccess("Scanning ...");
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    Inventory = Instance->GetInventory();
    

    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    checkf(SelectedRecipe.Requirements, TEXT("REQUIREMENT INVALID"))
    
    int32 AvailableCasing = 0;
    
    for (auto& Casing : Instance->GetInventory().CasingsStored)
    {
        if (Casing.Value.GetType() == SelectedRecipe.RequiredCasingType)
        {
            AvailableCasing = Casing.Value.GetQuantity();
            DebugHelper::LogWarning("Casing Quantities -> " + FString::FromInt(AvailableCasing));
            break;
        }
    }

    if (AvailableCasing < SelectedRecipe.Requirements->CasingQuantity)
    {
        DebugHelper::LogWarning("Not enough casing of correct type");
        return IsCraftable = false;
    }

    
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);

        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        if (!StoredEssence)
        {
            DebugHelper::LogError("Missing required essence: " + EssenceName);
            return IsCraftable = false;
        }

        if (StoredEssence->Quantity < RequiredEssence.Quantity)
        {
            DebugHelper::LogError("Not enough of essence: " + EssenceName);
            return IsCraftable = false;
        }
    }

    DebugHelper::LogSuccess("Item can be crafted!");
    return IsCraftable = true;
}

bool UCraftingTable::ScanResources(UWorld* World)
{
    DebugHelper::LogSuccess("Scanning ...");
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());

    if (!Instance)
    {
        DebugHelper::LogError("Game Instance is null!");
        return IsCraftable = false;
    }

    Inventory = Instance->GetInventory();
    

    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();


    checkf(SelectedRecipe.Requirements, TEXT("REQUIREMENT INVALID"))
    
    int32 AvailableCasing = 0;

    for (auto& Casing : Instance->GetInventory().CasingsStored)
    {
        if (Casing.Value.GetType() == SelectedRecipe.RequiredCasingType)
        {
            AvailableCasing = Casing.Value.GetQuantity();
            DebugHelper::LogWarning("Casing Quantities -> " + FString::FromInt(AvailableCasing));
            break;
        }
    }


    if (AvailableCasing < SelectedRecipe.Requirements->CasingQuantity)
    {
        DebugHelper::LogWarning("Not enough casing of correct type");
        return IsCraftable = false;
    }

    
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);

        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        if (!StoredEssence)
        {
            DebugHelper::LogError("Missing required essence: " + EssenceName);
            return IsCraftable = false;
        }

        if (StoredEssence->Quantity < RequiredEssence.Quantity)
        {
            DebugHelper::LogError("Not enough of essence: " + EssenceName);
            return IsCraftable = false;
        }
    }

    DebugHelper::LogSuccess("Item can be crafted!");
    return IsCraftable = true;
}

void UCraftingTable::Craft()
{
    if (!IsCraftable)
    {
        DebugHelper::LogError("Item not craftable");
        return;
    }

    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();

    int32 RemainingToConsume = SelectedRecipe.Requirements->CasingQuantity;

    if (SelectedRecipe.GetRequiredEssences().IsEmpty())
    {
        DebugHelper::LogError("Required essence is null");
        return;
    }
    
   // Iterate all stored casings to consume the required amount
    for (auto& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        //const FString& EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        const FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
    
        if (EssenceName.IsEmpty())
        {
            DebugHelper::LogError("Essence name is null");
            break;
        }
    
        if (const auto StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName))
        {
            const int32 CurrentEssenceStored = StoredEssence->Quantity;
            const int32 RequiredQuantity = RequiredEssence.Quantity;
    
            const int32 NewQuantity = CurrentEssenceStored - RequiredQuantity;
            if (NewQuantity < 0)
            {
                DebugHelper::LogError("Not enough " + EssenceName + " essence to craft.");
                continue; // or handle this error how you want
            }
    
            StoredEssence->Quantity = NewQuantity;
    
            DebugHelper::LogMessage(10, FColor::Cyan,
                "Consumed " + FString::FromInt(RequiredQuantity) +
                " " + EssenceName + ", remaining: " + FString::FromInt(NewQuantity));
        }
        else
        {
            DebugHelper::LogError("Required essence not found in inventory: " + EssenceName);
        }
    }
    
    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
    
        if (Casing.GetType() != SelectedRecipe.RequiredCasingType)
            continue; // Don't break — just skip
    
        const int32 CurrentQuantity = Casing.GetQuantity();
        const int32 RequiredQuantity = SelectedRecipe.Requirements->CasingQuantity;
    
        const int32 Cost = CurrentQuantity - RequiredQuantity;
        Casing.SetQuantity(Cost);
    
        DebugHelper::LogMessage(10, FColor::Cyan, "New quantity: " + FString::FromInt(Cost));
    }


    
    
    IsCraftable = false;
}

void UCraftingTable::Craft(UWorld* World)
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    if (SelectedRecipe.GetRequiredEssences().IsEmpty())
    {
        DebugHelper::LogError("Required essence is null");
        return;
    }

    for (auto& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        const FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
    
        if (EssenceName.IsEmpty())
        {
            DebugHelper::LogError("Essence name is null");
            break;
        }
    
        if (const auto StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName))
        {
            const int32 CurrentEssenceStored = StoredEssence->Quantity;
            const int32 RequiredQuantity = RequiredEssence.Quantity;
    
            const int32 NewQuantity = CurrentEssenceStored - RequiredQuantity;
            if (NewQuantity < 0)
            {
                DebugHelper::LogError("Not enough " + EssenceName + " essence to craft.");
                continue; // or handle this error how you want
            }
    
            StoredEssence->Quantity = NewQuantity;
    
            DebugHelper::LogMessage(10, FColor::Cyan,
                "Consumed " + FString::FromInt(RequiredQuantity) +
                " " + EssenceName + ", remaining: " + FString::FromInt(NewQuantity));
        }
        else
        {
            DebugHelper::LogError("Required essence not found in inventory: " + EssenceName);
        }
    }

    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
    
        if (Casing.GetType() != SelectedRecipe.RequiredCasingType)
            continue; // Don't break — just skip
    
        const int32 CurrentQuantity = Casing.GetQuantity();
        const int32 RequiredQuantity = SelectedRecipe.Requirements->CasingQuantity;
    
        const int32 Cost = CurrentQuantity - RequiredQuantity;
        Casing.SetQuantity(Cost);
    
        DebugHelper::LogMessage(10, FColor::Cyan, "New quantity: " + FString::FromInt(Cost));
    }
    
    IsCraftable = false;
}

FInventory UCraftingTable::GetInventory() const
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    checkf(Instance, TEXT("Instance invalid"))
    return Instance->GetInventory();
}


TArray<FRecipe> UCraftingTable::GetAvaiableRecipes()
{
    TArray<FRecipe> AvailableRecipes;

    DebugHelper::LogSuccess("GetAvailableRecipes called before loop");
    
    for (const FRecipe& R : GetInventory().Recipes)
    {
        DebugHelper::LogError("Joining the recipe loop");
        AvailableRecipes.Add(R);
    }

    DebugHelper::LogSuccess("GetAvailableRecipes called after loop");

    return AvailableRecipes;
}


// void UCraftingTable::SetInventoryReference(FInventory* InventoryRef)
// {
//     PlayerInventoryRef = InventoryRef;
// }


