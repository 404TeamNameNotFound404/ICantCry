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
    // ScanResources(World);
    // UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    // const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();


    // if (!SelectedRecipe.ResultBullet.GetBulletData())
    // {
    //     DebugHelper::LogError("Bullet data is null can't craft");
    //     return;
    // }
    
    // FBullet Bullet = SelectedRecipe.ResultBullet;
    // Instance->GetInventory().AddCraftedBullet(Bullet);

    // Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    // Craft(World);

    // FUNZIONA

    // if (!ScanResources(World))
    // {
    //     DebugHelper::LogError("Cannot craft - insufficient resources");
    //     return;
    // }
    
    // UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    // const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();

    // if (!SelectedRecipe.ResultBullet.GetBulletData())
    // {
    //     DebugHelper::LogError("Bullet data is null can't craft");
    //     return;
    // }
    
    // // 2. Esegui il craft (consuma risorse e aggiunge UN SOLO bullet)
    // Craft(World);

    // FUNZIONA



    // PROVA
    UE_LOG(LogTemp, Warning, TEXT("=== CRAFTSELECTEDBULLET CALLED ==="));
    
    // Controlla se ScanResources è chiamato due volte
    UE_LOG(LogTemp, Warning, TEXT("Calling ScanResources..."));
    ScanResources(World);
    
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    const FRecipe& SelectedRecipe = Instance->GetInventory().GetSelectedRecipe();

    if (!SelectedRecipe.ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Error, TEXT("Bullet data is null can't craft"));
        return;
    }
    
    // DEBUG: Verifica se qui aggiunge già un bullet
    UE_LOG(LogTemp, Warning, TEXT("About to add bullet in CraftSelectedBullet"));
    
    // COMMENTA QUESTA RIGA SE C'È - potrebbe essere che aggiunge già un bullet qui
    // FBullet Bullet = SelectedRecipe.ResultBullet;
    // Instance->GetInventory().AddCraftedBullet(Bullet);
    
    // Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    UE_LOG(LogTemp, Warning, TEXT("Calling Craft()..."));
    Craft(World);
    
    UE_LOG(LogTemp, Warning, TEXT("=== CRAFTSELECTEDBULLET FINISHED ==="));
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
//     if (!IsCraftable)
//     {
//         DebugHelper::LogError("Item not craftable");
//         return;
//     }

//     UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
//     FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();

//     int32 RemainingToConsume = SelectedRecipe.Requirements->CasingQuantity;

//     if (SelectedRecipe.GetRequiredEssences().IsEmpty())
//     {
//         DebugHelper::LogError("Required essence is null");
//         return;
//     }
    
//    // Iterate all stored casings to consume the required amount
//     for (auto& RequiredEssence : SelectedRecipe.RequiredEssences)
//     {
//         //const FString& EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
//         const FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
    
//         if (EssenceName.IsEmpty())
//         {
//             DebugHelper::LogError("Essence name is null");
//             break;
//         }
    
//         if (const auto StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName))
//         {
//             const int32 CurrentEssenceStored = StoredEssence->Quantity;
//             const int32 RequiredQuantity = RequiredEssence.Quantity;
    
//             const int32 NewQuantity = CurrentEssenceStored - RequiredQuantity;
//             if (NewQuantity < 0)
//             {
//                 DebugHelper::LogError("Not enough " + EssenceName + " essence to craft.");
//                 continue; // or handle this error how you want
//             }
    
//             StoredEssence->Quantity = NewQuantity;
    
//             DebugHelper::LogMessage(10, FColor::Cyan,
//                 "Consumed " + FString::FromInt(RequiredQuantity) +
//                 " " + EssenceName + ", remaining: " + FString::FromInt(NewQuantity));
//         }
//         else
//         {
//             DebugHelper::LogError("Required essence not found in inventory: " + EssenceName);
//         }
//     }
    
//     for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
//     {
//         FCasing& Casing = StoredCasing.Value;
    
//         if (Casing.GetType() != SelectedRecipe.RequiredCasingType)
//             continue; // Don't break — just skip
    
//         const int32 CurrentQuantity = Casing.GetQuantity();
//         const int32 RequiredQuantity = SelectedRecipe.Requirements->CasingQuantity;
    
//         const int32 Cost = CurrentQuantity - RequiredQuantity;
//         Casing.SetQuantity(Cost);
    
//         DebugHelper::LogMessage(10, FColor::Cyan, "New quantity: " + FString::FromInt(Cost));
//     }


    
    
//     IsCraftable = false;





    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (!Instance)
    {
        DebugHelper::LogError("Craft: GameInstance is null");
        return;
    }
    
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // DEBUG: Inizio
    DebugHelper::LogMessage(1, FColor::Yellow, "=== START CRAFT ===");
    
    // 1. Controlla se il craft è possibile
    if (!IsCraftable)
    {
        DebugHelper::LogError("Item not craftable");
        return;
    }
    
    // 2. Consuma le ESSENCE
    DebugHelper::LogMessage(1, FColor::Yellow, "Consuming essences...");
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        
        if (!StoredEssence)
        {
            DebugHelper::LogError("Essence not found: " + EssenceName);
            return;
        }
        
        // DEBUG
        DebugHelper::LogMessage(1, FColor::Cyan, 
            "Essence: " + EssenceName + 
            " - Before: " + FString::FromInt(StoredEssence->Quantity) +
            ", Need: " + FString::FromInt(RequiredEssence.Quantity));
        
        // Consuma SOLO la quantità richiesta
        StoredEssence->Quantity -= RequiredEssence.Quantity;
        
        // DEBUG
        DebugHelper::LogMessage(1, FColor::Cyan, 
            "Essence: " + EssenceName + 
            " - After: " + FString::FromInt(StoredEssence->Quantity));
        
        // Se finisce, rimuovi
        if (StoredEssence->Quantity <= 0)
        {
            Instance->GetInventory().EssencesStored.Remove(EssenceName);
        }
    }
    
    // 3. Consuma i CASING
    DebugHelper::LogMessage(1, FColor::Yellow, "Consuming casings...");
    
    int32 CasingsNeeded = SelectedRecipe.Requirements->CasingQuantity;
    ECasingType RequiredType = SelectedRecipe.RequiredCasingType;
    
    DebugHelper::LogMessage(1, FColor::Cyan, 
        "Need " + FString::FromInt(CasingsNeeded) + 
        " casing(s) of type: " + UEnum::GetValueAsString(RequiredType));
    
    bool bCasingsConsumed = false;
    
    // Cerca tutti i casing del tipo richiesto
    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
        
        // Salta se non è del tipo giusto
        if (Casing.GetType() != RequiredType)
            continue;
        
        int32 CurrentQuantity = Casing.GetQuantity();
        
        DebugHelper::LogMessage(1, FColor::Cyan, 
            "Found stack with " + FString::FromInt(CurrentQuantity) + " casings");
        
        // Se questo stack ha abbastanza casings
        if (CurrentQuantity >= CasingsNeeded)
        {
            // CALCOLA la nuova quantità
            int32 NewQuantity = CurrentQuantity - CasingsNeeded;
            
            // IMPOSTA la nuova quantità
            Casing.SetQuantity(NewQuantity);
            
            DebugHelper::LogMessage(1, FColor::Green, 
                "Consumed " + FString::FromInt(CasingsNeeded) + 
                " casing(s). Remaining in stack: " + FString::FromInt(NewQuantity));
            
            CasingsNeeded = 0; // Abbiamo consumato tutto il necessario
            bCasingsConsumed = true;
            break; // ESCO dal loop - ho finito!
        }
        else
        {
            // Questo stack non ha abbastanza, consumalo tutto e cerca nel prossimo
            DebugHelper::LogMessage(1, FColor::Yellow, 
                "Stack doesn't have enough. Consuming all " + 
                FString::FromInt(CurrentQuantity) + " from this stack");
            
            CasingsNeeded -= CurrentQuantity; // Aggiorna quanto ancora ci serve
            Casing.SetQuantity(0); // Consuma tutto questo stack
            
            // Continua con il prossimo stack
        }
    }
    
    // Controlla se abbiamo consumato abbastanza casings
    if (CasingsNeeded > 0)
    {
        DebugHelper::LogError("Not enough casings! Still need: " + FString::FromInt(CasingsNeeded));
        return;
    }
    
    if (!bCasingsConsumed)
    {
        DebugHelper::LogError("No casings of required type found!");
        return;
    }
    
    // 4. Aggiungi il BULLET risultato (SOLO 1!)
    DebugHelper::LogMessage(1, FColor::Yellow, "Adding crafted bullet...");
    
    FBullet ResultBullet = SelectedRecipe.ResultBullet;
    
    // DEBUG: mostra cosa stai creando
    if (ResultBullet.GetBulletData())
    {
        DebugHelper::LogMessage(1, FColor::Green, 
            "Crafted: " + ResultBullet.GetBulletData()->BulletName);
    }
    
    // Aggiungi all'inventario (SOLO 1!)
    Instance->GetInventory().AddCraftedBullet(ResultBullet);
    
    // 5. Notifica
    Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    // 6. Resetta
    IsCraftable = false;
    
    DebugHelper::LogSuccess("=== CRAFT COMPLETED SUCCESSFULLY ===");
}

void UCraftingTable::Craft(UWorld* World)
{
    // UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    // FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // if (SelectedRecipe.GetRequiredEssences().IsEmpty())
    // {
    //     DebugHelper::LogError("Required essence is null");
    //     return;
    // }

    // for (auto& RequiredEssence : SelectedRecipe.RequiredEssences)
    // {
    //     const FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
    
    //     if (EssenceName.IsEmpty())
    //     {
    //         DebugHelper::LogError("Essence name is null");
    //         break;
    //     }
    
    //     if (const auto StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName))
    //     {
    //         const int32 CurrentEssenceStored = StoredEssence->Quantity;
    //         const int32 RequiredQuantity = RequiredEssence.Quantity;
    
    //         const int32 NewQuantity = CurrentEssenceStored - RequiredQuantity;
    //         if (NewQuantity < 0)
    //         {
    //             DebugHelper::LogError("Not enough " + EssenceName + " essence to craft.");
    //             continue; // or handle this error how you want
    //         }
    
    //         StoredEssence->Quantity = NewQuantity;
    
    //         DebugHelper::LogMessage(10, FColor::Cyan,
    //             "Consumed " + FString::FromInt(RequiredQuantity) +
    //             " " + EssenceName + ", remaining: " + FString::FromInt(NewQuantity));
    //     }
    //     else
    //     {
    //         DebugHelper::LogError("Required essence not found in inventory: " + EssenceName);
    //     }
    // }

    // for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    // {
    //     FCasing& Casing = StoredCasing.Value;
    
    //     if (Casing.GetType() != SelectedRecipe.RequiredCasingType)
    //         continue; // Don't break — just skip
    
    //     const int32 CurrentQuantity = Casing.GetQuantity();
    //     const int32 RequiredQuantity = SelectedRecipe.Requirements->CasingQuantity;
    
    //     const int32 Cost = CurrentQuantity - RequiredQuantity;
    //     Casing.SetQuantity(Cost);
    
    //     DebugHelper::LogMessage(10, FColor::Cyan, "New quantity: " + FString::FromInt(Cost));
    // }
    
    // IsCraftable = false;

    // FUNZIONA

    // UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    // FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // // DEBUG: Inizio
    // DebugHelper::LogMessage(1, FColor::Yellow, "=== START CRAFT ===");
    
    // // 1. Controlla se il craft è possibile
    // if (!IsCraftable && !ScanResources(World))
    // {
    //     DebugHelper::LogError("Item not craftable");
    //     return;
    // }
    
    // // 2. Consuma le ESSENCE (1 per tipo, come nel tuo esempio)
    // DebugHelper::LogMessage(1, FColor::Yellow, "Consuming essences...");
    // for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    // {
    //     FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
    //     FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        
    //     if (!StoredEssence)
    //     {
    //         DebugHelper::LogError("Essence not found: " + EssenceName);
    //         return;
    //     }
        
    //     // DEBUG
    //     DebugHelper::LogMessage(1, FColor::Cyan, 
    //         "Essence: " + EssenceName + 
    //         " - Before: " + FString::FromInt(StoredEssence->Quantity) +
    //         ", Need: " + FString::FromInt(RequiredEssence.Quantity));
        
    //     // Consuma SOLO la quantità richiesta (probabilmente 1)
    //     StoredEssence->Quantity -= RequiredEssence.Quantity;
        
    //     // DEBUG
    //     DebugHelper::LogMessage(1, FColor::Cyan, 
    //         "Essence: " + EssenceName + 
    //         " - After: " + FString::FromInt(StoredEssence->Quantity));
        
    //     // Se finisce, rimuovi
    //     if (StoredEssence->Quantity <= 0)
    //     {
    //         Instance->GetInventory().EssencesStored.Remove(EssenceName);
    //     }
    // }
    
    // // 3. Consuma i CASING (probabilmente 1 solo)
    // DebugHelper::LogMessage(1, FColor::Yellow, "Consuming casings...");
    
    // int32 CasingsNeeded = SelectedRecipe.Requirements->CasingQuantity; // Probabilmente 1
    // ECasingType RequiredType = SelectedRecipe.RequiredCasingType;
    
    // DebugHelper::LogMessage(1, FColor::Cyan, 
    //     "Need " + FString::FromInt(CasingsNeeded) + 
    //     " casing(s) of type: " + UEnum::GetValueAsString(RequiredType));
    
    // bool bCasingsConsumed = false;
    
    // // Cerca tutti i casing del tipo richiesto
    // for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    // {
    //     FCasing& Casing = StoredCasing.Value;
        
    //     // Salta se non è del tipo giusto
    //     if (Casing.GetType() != RequiredType)
    //         continue;
        
    //     int32 CurrentQuantity = Casing.GetQuantity();
        
    //     DebugHelper::LogMessage(1, FColor::Cyan, 
    //         "Found stack with " + FString::FromInt(CurrentQuantity) + " casings");
        
    //     // Se questo stack ha abbastanza casings
    //     if (CurrentQuantity >= CasingsNeeded)
    //     {
    //         // CALCOLA la nuova quantità
    //         int32 NewQuantity = CurrentQuantity - CasingsNeeded;
            
    //         // IMPOSTA la nuova quantità
    //         Casing.SetQuantity(NewQuantity);
            
    //         DebugHelper::LogMessage(1, FColor::Green, 
    //             "Consumed " + FString::FromInt(CasingsNeeded) + 
    //             " casing(s). Remaining in stack: " + FString::FromInt(NewQuantity));
            
    //         CasingsNeeded = 0; // Abbiamo consumato tutto il necessario
    //         bCasingsConsumed = true;
    //         break; // ESCO dal loop - ho finito!
    //     }
    //     else
    //     {
    //         // Questo stack non ha abbastanza, consumalo tutto e cerca nel prossimo
    //         DebugHelper::LogMessage(1, FColor::Yellow, 
    //             "Stack doesn't have enough. Consuming all " + 
    //             FString::FromInt(CurrentQuantity) + " from this stack");
            
    //         CasingsNeeded -= CurrentQuantity; // Aggiorna quanto ancora ci serve
    //         Casing.SetQuantity(0); // Consuma tutto questo stack
            
    //         // Continua con il prossimo stack
    //     }
    // }
    
    // // Controlla se abbiamo consumato abbastanza casings
    // if (CasingsNeeded > 0)
    // {
    //     DebugHelper::LogError("Not enough casings! Still need: " + FString::FromInt(CasingsNeeded));
    //     return;
    // }
    
    // if (!bCasingsConsumed)
    // {
    //     DebugHelper::LogError("No casings of required type found!");
    //     return;
    // }
    
    // // 4. Aggiungi il BULLET risultato (SOLO 1!)
    // DebugHelper::LogMessage(1, FColor::Yellow, "Adding crafted bullet...");
    
    // FBullet ResultBullet = SelectedRecipe.ResultBullet;
    
    // // DEBUG: mostra cosa stai creando
    // if (ResultBullet.GetBulletData())
    // {
    //     DebugHelper::LogMessage(1, FColor::Green, 
    //         "Crafted: " + ResultBullet.GetBulletData()->BulletName);
    // }
    
    // // Aggiungi all'inventario (SOLO 1!)
    // Instance->GetInventory().AddCraftedBullet(ResultBullet);
    
    // // 5. Notifica
    // Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    // // 6. Resetta
    // IsCraftable = false;
    
    // DebugHelper::LogSuccess("=== CRAFT COMPLETED SUCCESSFULLY ===");

    // FUNZIONA




    // PROVA 

     UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    FRecipe& SelectedRecipe = Inventory.GetSelectedRecipe();
    
    // LOG DETTAGLIATO PRIMA
    UE_LOG(LogTemp, Warning, TEXT("=== BEFORE CRAFT ==="));
    UE_LOG(LogTemp, Warning, TEXT("Recipe has %d required essences"), SelectedRecipe.RequiredEssences.Num());
    
    // Controlla TUTTE le essence nell'inventario prima
    for (const auto& EssencePair : Instance->GetInventory().EssencesStored)
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventory Essence: %s = %d"), 
            *EssencePair.Key, 
            EssencePair.Value.Quantity);
    }
    
    // Controlla cosa richiede la recipe
    for (int32 i = 0; i < SelectedRecipe.RequiredEssences.Num(); i++)
    {
        const FEssence& RequiredEssence = SelectedRecipe.RequiredEssences[i];
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        
        UE_LOG(LogTemp, Warning, TEXT("Recipe requires [%d]: %s x%d"), 
            i, 
            *EssenceName, 
            RequiredEssence.Quantity);
    }
    
    // 1. Consuma le ESSENCE
    UE_LOG(LogTemp, Warning, TEXT("=== CONSUMING ESSENCES ==="));
    
    for (const FEssence& RequiredEssence : SelectedRecipe.RequiredEssences)
    {
        FString EssenceName = RequiredEssence.GetName(RequiredEssence.EssenceType);
        FEssence* StoredEssence = Instance->GetInventory().EssencesStored.Find(EssenceName);
        
        if (!StoredEssence)
        {
            UE_LOG(LogTemp, Error, TEXT("Essence not found: %s"), *EssenceName);
            return;
        }
        
        // LOG IMPORTANTE
        UE_LOG(LogTemp, Warning, TEXT("CONSUMING: %s | Before: %d | Need: %d | After: %d"), 
            *EssenceName,
            StoredEssence->Quantity,
            RequiredEssence.Quantity,
            StoredEssence->Quantity - RequiredEssence.Quantity);
        
        // Consuma
        StoredEssence->Quantity -= RequiredEssence.Quantity;
        
        if (StoredEssence->Quantity <= 0)
        {
            Instance->GetInventory().EssencesStored.Remove(EssenceName);
            UE_LOG(LogTemp, Warning, TEXT("Removed %s from inventory (quantity <= 0)"), *EssenceName);
        }
    }
    
    // LOG DETTAGLIATO DOPO
    UE_LOG(LogTemp, Warning, TEXT("=== AFTER CRAFT ==="));
    for (const auto& EssencePair : Instance->GetInventory().EssencesStored)
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventory Essence: %s = %d"), 
            *EssencePair.Key, 
            EssencePair.Value.Quantity);
    }
    
    // 2. Consuma i CASING
    UE_LOG(LogTemp, Warning, TEXT("=== CONSUMING CASINGS ==="));
    
    int32 CasingsNeeded = SelectedRecipe.Requirements->CasingQuantity;
    ECasingType RequiredType = SelectedRecipe.RequiredCasingType;
    
    UE_LOG(LogTemp, Warning, TEXT("Need %d casings of type: %s"), 
        CasingsNeeded, 
        *UEnum::GetValueAsString(RequiredType));
    
    bool bCasingsConsumed = false;
    
    for (auto& StoredCasing : Instance->GetInventory().CasingsStored)
    {
        FCasing& Casing = StoredCasing.Value;
        
        if (Casing.GetType() != RequiredType)
            continue;
        
        int32 CurrentQuantity = Casing.GetQuantity();
        
        UE_LOG(LogTemp, Warning, TEXT("Found stack with %d casings"), CurrentQuantity);
        
        if (CurrentQuantity >= CasingsNeeded)
        {
            int32 NewQuantity = CurrentQuantity - CasingsNeeded;
            Casing.SetQuantity(NewQuantity);
            
            UE_LOG(LogTemp, Warning, TEXT("Consumed %d casing(s). Remaining in stack: %d"), 
                CasingsNeeded, 
                NewQuantity);
            
            CasingsNeeded = 0;
            bCasingsConsumed = true;
            break;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Stack doesn't have enough. Consuming all %d from this stack"), 
                CurrentQuantity);
            
            CasingsNeeded -= CurrentQuantity;
            Casing.SetQuantity(0);
        }
    }
    
    if (CasingsNeeded > 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Not enough casings! Still need: %d"), CasingsNeeded);
        return;
    }
    
    if (!bCasingsConsumed)
    {
        UE_LOG(LogTemp, Error, TEXT("No casings of required type found!"));
        return;
    }
    
    // 3. Aggiungi il BULLET risultato (SOLO 1!)
    FBullet ResultBullet = SelectedRecipe.ResultBullet;
    
    if (ResultBullet.GetBulletData())
    {
        UE_LOG(LogTemp, Warning, TEXT("Crafted: %s"), *ResultBullet.GetBulletData()->BulletName);
    }
    
    // Aggiungi all'inventario (SOLO 1!)
    Instance->GetInventory().AddCraftedBullet(ResultBullet);
    
    // 4. Notifica
    Instance->GetInventory().OnBulletCrafted.Broadcast();
    
    // 5. Resetta
    IsCraftable = false;
    
    UE_LOG(LogTemp, Warning, TEXT("=== CRAFT COMPLETED SUCCESSFULLY ==="));


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


