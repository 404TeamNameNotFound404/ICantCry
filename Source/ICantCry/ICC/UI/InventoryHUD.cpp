// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHUD.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"


void UInventoryHUD::NativeConstruct()
{
    Super::NativeConstruct();
    CurrentSelectedIndex = -1;  
}

void UInventoryHUD::UpdateInventoryDisplay(const FInventory& Inventory)
{
    ClearBulletList();
    DebugHelper::LogWarning("UpdateInventoryDisplay chiamato!");
    PopulateBulletList(Inventory.Items);
    
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

       //Player->GetBattleHUD()->MinigameSlot->AddChild(CurrentMinigameDisplayed);

        // if (BulletButton)
        // {
            BulletButton->Setup(Item.Bullet, Item.Quantity);   // imposta nome, icona, quantità
            BulletButton->SetOwner(this, Index);               // collega all'HUD + index

            BulletListContainer->AddChild(BulletButton);       // aggiungi visivamente
            BulletButtons.Add(BulletButton);                   // salva riferimento per selezione
            DisplayedBullets.Add(Item.Bullet);                 // salva i dati del proiettile

            Index++;
        // }
    }

    // Se ci sono proiettili, seleziona il primo
    if (DisplayedBullets.Num() > 0)
    {
        SelectBullet(0);
    }
    
    
}




void UInventoryHUD::SelectBullet(int32 Index)
{
   if (Index < 0 || Index >= DisplayedBullets.Num()) return;

    // Reset all selections
    for (UBulletBottonItem* Button : BulletButtons)
    {
        if (Button) Button->SetSelected(false);
    }

    // Highlight selected
    if (BulletButtons.IsValidIndex(Index))
    {
        BulletButtons[Index]->SetSelected(true);
        CurrentSelectedIndex = Index;
        UpdateDetailPanel(DisplayedBullets[Index]);
    }
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


