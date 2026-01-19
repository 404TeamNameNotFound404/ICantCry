// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryHUD.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/UI/CasingWidget.h"

void UInventoryHUD::NativeConstruct()
{
    Super::NativeConstruct();

    CraftingTable = NewObject<UCraftingTable>();
    CraftingTable->Initialize(GetWorld());
    
    CurrentSelectedIndex = -1;

    GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    checkf(GameInstance, TEXT("game instance invalid"))

    ImmutableInventory = GameInstance->GetInventory();

    CraftButton->OnClicked.AddDynamic(this, &UInventoryHUD::OnCraftClicked);

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

void UInventoryHUD::OnCraftClicked()
{
    CraftingTable->CraftSelectedBullet(GetWorld());
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





