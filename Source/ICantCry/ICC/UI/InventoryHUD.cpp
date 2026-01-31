// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryHUD.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/UI/CasingWidget.h"

void UInventoryHUD::NativeConstruct()
{   

    GetWorld()->GetTimerManager().ClearTimer(Timer);
    Super::NativeConstruct();

    

    CraftingTable = NewObject<UCraftingTable>();
    CraftingTable->Initialize(GetWorld());
    
    CurrentSelectedIndex = -1;

    GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    checkf(GameInstance, TEXT("game instance invalid"))

    ImmutableInventory = GameInstance->GetInventory();

    ButtonSwitcher->OnClicked.AddDynamic(this, &UInventoryHUD::OnToggleSwitcher);
    
    
    CraftButton->OnPressed.AddDynamic(this, &UInventoryHUD::OnCraftPressed);
    CraftButton->OnReleased.AddDynamic(this, &UInventoryHUD::OnCraftReleased);

    if (CraftingProgressBar)
    {
        CraftingProgressBar->SetPercent(0.0f);
    }

    //StandardBulletDisplayer->Init(this);
}

void UInventoryHUD::UpdateInventoryDisplay(const FInventory &Inventory)
{
}

void UInventoryHUD::ClearBulletList()
{
    // if(BulletBox)
    // {
    //     BulletBox->ClearChildren();
    // }

    // DisplayedBullets.Empty(); // svuota i dati visualizzati
    // BulletButtons.Empty();
}

void UInventoryHUD::SelectBullet(int32 Index)
{
    //    if (GameInstance->GetInventory().BulletsStored.IsEmpty())
    //    {
    //        return;
    //    }

    //     for (auto& Bullet : GameInstance->GetInventory().BulletsStored)
    //     {
    //         FBullet& B = Bullet.Value;

    //     }

    // // Reset all selections
    // for (UBulletBottonItem* Button : BulletButtons)
    // {
    //     if (Button) Button->SetSelected(false);
    // }
    //
    // // Highlight selected
    // if (BulletButtons.IsValidIndex(Index))
    // {
    //     BulletButtons[Index]->SetSelected(true);
    //     CurrentSelectedIndex = Index;
    //     UpdateDetailPanel(DisplayedBullets[Index]);
    // }
}

void UInventoryHUD::Refresh()
{
    // DisplayCasings();
    // DisplayBullets();
    RefreshEssence();
}

void UInventoryHUD::UpdateDetailPanel(const FBullet &Bullet)
{
    if (!Bullet.GetBulletData())
        return;

    if (SelectedBulletName)
    {
        SelectedBulletName->SetText(FText::FromString(Bullet.GetBulletData()->BulletName));
    }

    if (SelectedBulletPower)
    {
        SelectedBulletPower->SetText(FText::AsNumber(Bullet.GetBulletData()->Power));
    }
}

void UInventoryHUD::MoveSelectionUp()
{
    if (DisplayedBullets.Num() > 0)
    {
        int32 NewIndex = (CurrentSelectedIndex - 1 + DisplayedBullets.Num()) % DisplayedBullets.Num();
        SelectBullet(NewIndex);
    }
}

void UInventoryHUD::MoveSelectionDown()
{
    if (DisplayedBullets.Num() > 0)
    {
        int32 NewIndex = (CurrentSelectedIndex + 1) % DisplayedBullets.Num();
        SelectBullet(NewIndex);
    }
}

FText UInventoryHUD::OnQuantityChanged()
{
    for (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
       const auto& Pair : Instance->GetInventory().CasingsStored)
    {
        const FString Txt = FString("x " + FString::FromInt(Pair.Value.GetQuantity()));
        return FText(FText::FromString(Txt));
    }

    return FText::FromString("x 0");
}

FText UInventoryHUD::OnGoldQuantityChanged()
{
    for (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
     const auto& Pair : Instance->GetInventory().GoldCasings)
    {
        const FString Txt = FString("x " + FString::FromInt(Pair.GetQuantity()));
        return FText(FText::FromString(Txt));
    }

    return FText::FromString("x 0");
}

void UInventoryHUD::OnToggleSwitcher()
{
    if (!BulletSwitcher) return; 
    
    int32 Current = BulletSwitcher->GetActiveWidgetIndex(); 
    int32 Next = (Current == 0) ? 1 : 0; 
    BulletSwitcher->SetActiveWidgetIndex(Next);
}



// PROGRESS BAR

void UInventoryHUD::OnCraftPressed()
{
    if (!CraftingTable || !GetWorld()) 
    {
        return;
    }
    
   
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
    if (!Instance) 
    {
        return;
    }
    
  
    FRecipe& CurrentRecipe = Instance->GetInventory().GetSelectedRecipe();
    if (CurrentRecipe.Requirements == nullptr) // <-- QUESTO È IL CONTROLLO CHIAVE
    {
        DebugHelper::LogError("No recipe selected!");
        
        // Feedback opzionale
        if (CraftInfo)
        {
            CraftInfo->SetText(FText::FromString("Select recipe first"));
        }
        
        return; 
    }
    
   
    if (CraftingTable->ScanResources(GetWorld()))
    {
        bIsHolding = true;
        CurrentProgress = 0.0f;
        
        GetWorld()->GetTimerManager().ClearTimer(Timer);
        GetWorld()->GetTimerManager().SetTimer(
            Timer, 
            this, 
            &UInventoryHUD::UpdateProgressBar, 
            0.02f,
            true
        );
    }

}

void UInventoryHUD::OnCraftReleased()
{
    if (!bIsHolding)
    {
        return;
    }

    bIsHolding = false;

    GetWorld()->GetTimerManager().ClearTimer(Timer);

    CurrentProgress = 0.0f;


   if (CraftingProgressBar)
    {
        CraftingProgressBar->SetPercent(0.0f);
    }
    
    DebugHelper::LogWarning("Progress bar reset to 0");


}

void UInventoryHUD::UpdateProgressBar()
{
   
    if (!bIsHolding)
    {
        return;
    }
    
    CurrentProgress += ProgressBarSpeed * 0.02f;
    
    
    if (CurrentProgress > 1.0f)
    {
        CurrentProgress = 1.0f;
    }
    
    if (CraftingProgressBar)
    {
        CraftingProgressBar->SetPercent(CurrentProgress);
    }
    
    
    if (CurrentProgress >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(Timer);
        CompleteCrafting();
    }
}

void UInventoryHUD::CompleteCrafting()
{
    if (!bIsHolding)
    {
        CurrentProgress = 0.0f;
        if (CraftingProgressBar)
        {
            CraftingProgressBar->SetPercent(0.0f);
        }
        return;
    }
    
    DebugHelper::LogSuccess("Crafting completed! Crafting 1 bullet...");
    
    // 1. Crafta UN SOLO bullet
    CraftingTable->CraftSelectedBullet(GetWorld());
    Refresh();
    
    // 2. Controlla se l'utente sta ancora tenendo premuto
    if (!bIsHolding)
    {
        // Utente ha rilasciato, resetta tutto
        CurrentProgress = 0.0f;
        if (CraftingProgressBar)
        {
            CraftingProgressBar->SetPercent(0.0f);
        }
        return;
    }
    
    // 3. Controlla se ci sono ancora risorse per craftare un altro bullet
    if (!CraftingTable->ScanResources(GetWorld()))
    {
        DebugHelper::LogWarning("No more resources available");
        
        // Nessuna risorsa, resetta tutto
        bIsHolding = false;
        CurrentProgress = 0.0f;
        if (CraftingProgressBar)
        {
            CraftingProgressBar->SetPercent(0.0f);
        }
        return;
    }
    
    // 4. Se l'utente tiene ancora premuto E ci sono risorse...
    //    Ricomincia la progress bar da 0 per craftare il prossimo bullet
    DebugHelper::LogWarning("User still holding - starting next bullet craft...");
    
    CurrentProgress = 0.0f;
    if (CraftingProgressBar)
    {
        CraftingProgressBar->SetPercent(0.0f);
    }
    
    // Ricomincia il timer per la prossima progress bar
    GetWorld()->GetTimerManager().ClearTimer(Timer);
    GetWorld()->GetTimerManager().SetTimer(
        Timer, 
        this, 
        &UInventoryHUD::UpdateProgressBar, 
        0.02f,
        true
    );
}


UCraftingTable* UInventoryHUD::GetTable()
{
    return CraftingTable;
}

void UInventoryHUD::RefreshEssence()
{
    EssenceBox->ClearChildren();

    for (const auto &Essence : GameInstance->GetInventory().EssencesStored)
    {
        UEssenceWidget *EssenceWidget = CreateWidget<UEssenceWidget>(GetWorld(), UEssenceWidgetClass);
        checkf(EssenceWidget, TEXT("Essence widget is null"))

        EssenceBox->AddChild(EssenceWidget);
        const FEssence &E = Essence.Value;
        EssenceWidget->Setup(E, E.Quantity);
    }
}

void UInventoryHUD::Setup()
{
    StandardBulletDisplayer->Init(this);
    StandardBulletDisplayer->Refresh();
    DebugHelper::LogWarning("Setup InventoryHUD called");
}





