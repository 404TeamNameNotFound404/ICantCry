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

    UE_LOG(LogTemp, Warning, TEXT("CraftSelectedBullet called"));
    
    ScanResources(World);
    
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();

    if (!SelectedRecipe.ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Error, TEXT("Bullet data is null"));
        return;
    }
    
    Craft(World);
    
    UE_LOG(LogTemp, Warning, TEXT("CraftSelectedBullet finished"));



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
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (!Instance)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance is null"));
        return;
    }
    
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // LOG: Inizio craft
    UE_LOG(LogTemp, Warning, TEXT("--- CRAFT START ---"));
    
    if (SelectedRecipe.ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Warning, TEXT("Crafting: %s"), 
            *SelectedRecipe.ResultBullet.GetBulletData()->BulletName);
    }
    
    if (!IsCraftable)
    {
        UE_LOG(LogTemp, Error, TEXT("Item not craftable"));
        return;
    }
    
    // 1. Consuma essences
    UE_LOG(LogTemp, Warning, TEXT("Essences:"));
    
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        
        if (!StoredEssence)
        {
            UE_LOG(LogTemp, Error, TEXT("Missing essence: %s"), *EssenceName);
            return;
        }
        
        int32 Before = StoredEssence->Quantity;
        int32 After = Before - RequiredEssence.Quantity;
        
        UE_LOG(LogTemp, Warning, TEXT("  %s: %d -> %d"), 
            *EssenceName, Before, After);
        
        StoredEssence->Quantity = After;
        
        if (StoredEssence->Quantity <= 0)
        {
            Instance->GetInventory().EssencesStored.Remove(EssenceName);
        }
    }
    
    // 2. Consuma casings
    UE_LOG(LogTemp, Warning, TEXT("Casings:"));
    
    int32 CasingsNeeded = SelectedRecipe.Requirements->CasingQuantity;
    ECasingType RequiredType = SelectedRecipe.RequiredCasingType;
    
    UE_LOG(LogTemp, Warning, TEXT("  Needed: %d %s"), 
        CasingsNeeded, 
        *UEnum::GetValueAsString(RequiredType));
    
    bool bCasingsConsumed = false;
    
    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
        
        if (Casing.GetType() != RequiredType)
            continue;
        
        int32 Before = Casing.GetQuantity();
        
        if (Before >= CasingsNeeded)
        {
            int32 After = Before - CasingsNeeded;
            Casing.SetQuantity(After);
            
            UE_LOG(LogTemp, Warning, TEXT("  %s: %d -> %d"), 
                *UEnum::GetValueAsString(RequiredType),
                Before, After);
            
            CasingsNeeded = 0;
            bCasingsConsumed = true;
            break;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("  Consuming all %d from this stack"), Before);
            
            CasingsNeeded -= Before;
            Casing.SetQuantity(0);
        }
    }
    
    if (CasingsNeeded > 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Not enough casings, still need: %d"), CasingsNeeded);
        return;
    }
    
    if (!bCasingsConsumed)
    {
        UE_LOG(LogTemp, Error, TEXT("No casings of required type found"));
        return;
    }
    
    // 3. Aggiungi bullet
    FBullet ResultBullet = SelectedRecipe.ResultBullet;
    
    if (ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Warning, TEXT("Created: %s"), *ResultBullet.GetBulletData()->BulletName);
    }
    
    Instance->GetInventory().AddCraftedBullet(ResultBullet);
    
    Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    IsCraftable = false;
    
    UE_LOG(LogTemp, Warning, TEXT("--- CRAFT END ---"));
}

void UCraftingTable::Craft(UWorld* World)
{

    

    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // LOG: Inizio craft
    UE_LOG(LogTemp, Warning, TEXT("--- CRAFT START ---"));
    
    if (SelectedRecipe.ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Warning, TEXT("Crafting: %s"), 
            *SelectedRecipe.ResultBullet.GetBulletData()->BulletName);
    }
    
    // 1. Consuma essences
    UE_LOG(LogTemp, Warning, TEXT("Essences:"));
    
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        
        if (!StoredEssence)
        {
            UE_LOG(LogTemp, Error, TEXT("Missing essence: %s"), *EssenceName);
            return;
        }
        
        int32 Before = StoredEssence->Quantity;
        int32 After = Before - RequiredEssence.Quantity;
        
        UE_LOG(LogTemp, Warning, TEXT("  %s: %d -> %d"), 
            *EssenceName, Before, After);
        
        // Consuma
        StoredEssence->Quantity = After;
        
        if (StoredEssence->Quantity <= 0)
        {
            Instance->GetInventory().EssencesStored.Remove(EssenceName);
        }
    }
    
    // 2. Consuma casings
    UE_LOG(LogTemp, Warning, TEXT("Casings:"));
    
    int32 CasingsNeeded = SelectedRecipe.Requirements->CasingQuantity;
    ECasingType RequiredType = SelectedRecipe.RequiredCasingType;
    
    UE_LOG(LogTemp, Warning, TEXT("  Needed: %d %s"), 
        CasingsNeeded, 
        *UEnum::GetValueAsString(RequiredType));
    
    bool bCasingsConsumed = false;
    
    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
        
        if (Casing.GetType() != RequiredType)
            continue;
        
        int32 Before = Casing.GetQuantity();
        
        if (Before >= CasingsNeeded)
        {
            int32 After = Before - CasingsNeeded;
            Casing.SetQuantity(After);
            
            UE_LOG(LogTemp, Warning, TEXT("  %s: %d -> %d"), 
                *UEnum::GetValueAsString(RequiredType),
                Before, After);
            
            CasingsNeeded = 0;
            bCasingsConsumed = true;
            break;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("  Consuming all %d from this stack"), Before);
            
            CasingsNeeded -= Before;
            Casing.SetQuantity(0);
        }
    }
    
    if (CasingsNeeded > 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Not enough casings, still need: %d"), CasingsNeeded);
        return;
    }
    
    if (!bCasingsConsumed)
    {
        UE_LOG(LogTemp, Error, TEXT("No casings of required type found"));
        return;
    }
    
    // 3. Aggiungi bullet
    FBullet ResultBullet = SelectedRecipe.ResultBullet;
    
    if (ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Warning, TEXT("Created: %s"), *ResultBullet.GetBulletData()->BulletName);
    }
    
    // Aggiungi all'inventario
    Instance->GetInventory().AddCraftedBullet(ResultBullet);
    
    // Notifica
    Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    IsCraftable = false;
    
    UE_LOG(LogTemp, Warning, TEXT("--- CRAFT END ---"));


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


