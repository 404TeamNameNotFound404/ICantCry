// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryHUD.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/UI/CasingWidget.h"


void UInventoryHUD::NativeConstruct()
{
    Super::NativeConstruct();
    CurrentSelectedIndex = -1;

    GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    checkf(GameInstance, TEXT("game instance invalid"))

    ImmutableInventory = GameInstance->GetInventory();
}

void UInventoryHUD::UpdateInventoryDisplay(const FInventory& Inventory)
{
    ClearBulletList();
    DebugHelper::LogWarning("UpdateInventoryDisplay chiamato!");
    PopulateBulletList(Inventory.Items);
}


void UInventoryHUD::DisplayCasings()
{
    DebugHelper::LogError("CASING SIZE " + FString::FromInt(ImmutableInventory.GoldCasings.Num()));
    BulletListContainer->ClearChildren();
    
    // for (const auto& Casing : ImmutableInventory.GoldCasings)
    // {
    //     DebugHelper::LogMessage(5, FColor::Magenta, "Found " + Casing.GetName());
    //
    //     UCasingWidget* CasingSlot = CreateWidget<UCasingWidget>(GetWorld(), CasingWidgetClass);
    //     // must put some check like Casing->Type == Type then increase its quantity without adding a new one
    //     CasingSlot->GetCasingImage()->SetBrushFromTexture(Casing.GetIcon());
    //     CasingSlot->GetCaseName()->SetText(FText::FromString(Casing.GetName() + " " + FString::FromInt(Casing.GetQuantity()) + "x"));
    //
    //     if (Casing.GetName() == ImmutableInventory.CasingsStored[Casing.GetName()].GetName())
    //     {
    //         CasingSlot->GetCaseName()->SetText(FText::FromString(Casing.GetName() + " " + FString::FromInt(ImmutableInventory.CasingsStored[Casing.GetName()].GetQuantity()) + "x"));
    //     }
    //     
    //     BulletListContainer->AddChild(CasingSlot);
    //     break;
    // }

    // for (const auto& Pair : ImmutableInventory.CasingsStored)
    // {
    //     const auto& Casing = Pair.Value;
    //
    //     UCasingWidget* CasingSlot = CreateWidget<UCasingWidget>(GetWorld(), CasingWidgetClass);
    //     CasingSlot->GetCasingImage()->SetBrushFromTexture(Casing.GetIcon());
    //
    //     FString Label = Casing.GetName() + " " + FString::FromInt(Casing.GetQuantity()) + "x";
    //     CasingSlot->GetCaseName()->SetText(FText::FromString(Label));
    //
    //     CasingSlot->SetPadding(FMargin{0, 2.5f});
    //     BulletListContainer->AddChild(CasingSlot);
    // }
}

void UInventoryHUD::ClearBulletList()
{
    if(BulletListContainer)
    {
        BulletListContainer->ClearChildren();
    }
    
    DisplayedBullets.Empty(); // svuota i dati visualizzati
    BulletButtons.Empty();
}


void UInventoryHUD::PopulateBulletList(const TArray<FInventoryItem>& Items)
{
    ClearBulletList();

    int32 Index = 0;

    for (const FInventoryItem& Item : Items)
    {
        if (Item.ItemType != EItemType::Bullet) continue;

        if (!BulletListContainer)
        {
            DebugHelper::LogError("BulletListContainer è null!");
            return;
        }

        //AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

        UBulletBottonItem* BulletButton = CreateWidget<UBulletBottonItem>(this, BulletButtonClass);


        if (!BulletButton)
        {
            DebugHelper::LogError("Failed to create BulletButton widget.");
            continue;
        }
        
            BulletButton->Setup(Item.Bullet, Item.Quantity);   // imposta nome, icona, quantità
            BulletButton->SetOwner(this, Index);               // collega all'HUD + index

            BulletListContainer->AddChild(BulletButton);       // aggiungi visivamente
            BulletButtons.Add(BulletButton);                   // salva riferimento per selezione
            DisplayedBullets.Add(Item.Bullet);                 // salva i dati del proiettile

            Index++;
    }

    // Se ci sono proiettili, seleziona il primo
    if (DisplayedBullets.Num() > 0)
    {
        SelectBullet(0);
    }
}

void UInventoryHUD::DisplayBullets()
{
    if (GameInstance->GetInventory().Bullets.IsEmpty())
    {
        return;
    }
    
    BulletListContainer->ClearChildren();
    
    for (auto& StoredBullet : GameInstance->GetInventory().BulletsStored)
    {
        FBullet& Bullet = StoredBullet.Value;
        UBulletBottonItem* Item = CreateWidget<UBulletBottonItem>(this, BulletButtonClass);
        Item->Setup(Bullet.GetBulletData(), Bullet.GetQuantity());
        Item->SetOwner(this, 0);
        BulletListContainer->AddChild(Item);
    }

    DebugHelper::LogError("Bullet refreshed");
}


void UInventoryHUD::SelectBullet(int32 Index)
{
   if (GameInstance->GetInventory().BulletsStored.IsEmpty())
   {
       return;
   }

    for (auto& Bullet : GameInstance->GetInventory().BulletsStored)
    {
        FBullet& B = Bullet.Value;

        
    }

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
    DisplayCasings();
    DisplayBullets();
}

void UInventoryHUD::UpdateDetailPanel(const FBullet& Bullet)
{
    if (!Bullet.GetBulletData()) return;

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


