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
    ButtonSwitcher->OnClicked.AddDynamic(this, &UInventoryHUD::OnToggleSwitcher);


    CraftButton->OnReleased.AddDynamic(this, &UInventoryHUD::OnCraftReleased);

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

void UInventoryHUD::UpdateCraft()
{
    CurrentProgress += ProgressBarSpeed * 0.02f;

    CraftingProgressBar->SetPercent(CurrentProgress);

    if(CurrentProgress >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(Timer);
        CraftingProgressBar->SetPercent(0.0f);
        CraftingTable->CraftSelectedBullet(GetWorld());
    }


}

void UInventoryHUD::OnCraftClicked()
{

    bIsHolding = true;

    CurrentProgress = 0;

    //GetWorld()->GetTimerManager().ClearTimer(Timer);
    GetWorld()->GetTimerManager().SetTimer(Timer, this, &UInventoryHUD::UpdateCraft, 0.05f, true);
    //CraftingTable->CraftSelectedBullet(GetWorld());

}

void UInventoryHUD::OnCraftReleased()
{
    bIsHolding = false;
    GetWorld()->GetTimerManager().ClearTimer(Timer);
    CraftingProgressBar->SetPercent(0.0f);
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





