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

    for (UBulletBottonItem *Btn : BulletButtons)
    {

        if (!Btn)
            continue;

        Btn->GetBulletButton()->SetIsEnabled(false);
        Btn->GetBulletIcon()->SetVisibility(ESlateVisibility::Hidden);
        Btn->GetBulletName()->SetVisibility(ESlateVisibility::Hidden);
        Btn->GetBulletQuantity()->SetVisibility(ESlateVisibility::Hidden);
        DebugHelper::LogSuccess("Name : " + Btn->GetName());
    }
}

void UInventoryHUD::UpdateInventoryDisplay(const FInventory &Inventory)
{
    // ClearBulletList();
    // DebugHelper::LogWarning("UpdateInventoryDisplay chiamato!");
    // PopulateBulletList(Inventory.Items);
}

void UInventoryHUD::DisplayCasings()
{
    // DebugHelper::LogError("CASING SIZE " + FString::FromInt(ImmutableInventory.GoldCasings.Num()));
    // BulletBox->ClearChildren();
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

void UInventoryHUD::PopulateBulletList(const TArray<FInventoryItem> &Items)
{
    // ClearBulletList();

    // int32 Index = 0;

    // for (const FInventoryItem& Item : Items)
    // {
    //     if (Item.ItemType != EItemType::Bullet) continue;

    //     if (!BulletBox)
    //     {
    //         DebugHelper::LogError("BulletListContainer è null!");
    //         return;
    //     }

    //     //AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

    //     UBulletBottonItem* BulletButton = CreateWidget<UBulletBottonItem>(this, BulletButtonClass);

    //     if (!BulletButton)
    //     {
    //         DebugHelper::LogError("Failed to create BulletButton widget.");
    //         continue;
    //     }

    //         BulletButton->Setup(Item.Bullet, Item.Quantity);   // imposta nome, icona, quantità
    //         BulletButton->SetOwner(this, Index);               // collega all'HUD + index

    //         BulletBox->AddChild(BulletButton);       // aggiungi visivamente
    //         BulletButtons.Add(BulletButton);                   // salva riferimento per selezione
    //         DisplayedBullets.Add(Item.Bullet);                 // salva i dati del proiettile

    //         Index++;
    // }

    // // Se ci sono proiettili, seleziona il primo
    // if (DisplayedBullets.Num() > 0)
    // {
    //     SelectBullet(0);
    // }
}

void UInventoryHUD::DisplayBullets()
{
    // if (GameInstance->GetInventory().Bullets.IsEmpty())
    // {
    //     return;
    // }

    // BulletBox->ClearChildren();

    // for (auto& StoredBullet : GameInstance->GetInventory().BulletsStored)
    // {
    //     FBullet& Bullet = StoredBullet.Value;
    //     UBulletBottonItem* Item = CreateWidget<UBulletBottonItem>(this, BulletButtonClass);
    //     Item->Setup(Bullet.GetBulletData(), Bullet.GetQuantity());
    //     Item->SetOwner(this, 0);
    //     BulletBox->AddChild(Item);
    // }

    // DebugHelper::LogError("Bullet refreshed");
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
    // RefreshEssence();
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
