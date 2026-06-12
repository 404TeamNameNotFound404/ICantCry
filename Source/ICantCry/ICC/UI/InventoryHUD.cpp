// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryHUD.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"

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
    
    if (DebugHelper::IsGamepadPlugged())
    {
        CraftIcon->SetBrushFromTexture(GameInstance->GetIconMap()["OPad_X"]);
    }
    else
    {
        CraftIcon->SetBrushFromTexture(GameInstance->GetIconMap()["OKey_MouseClicked"]);
    }
    
    FTimerHandle BindDelay;
    GetWorld()->GetTimerManager().SetTimer(BindDelay, this, &UInventoryHUD::Bind, 0.4f, false);
    
    BulletSwitcher->SetFocus();
    StandardBulletDisplayer->SetIsFocusable(true);
    StandardBulletDisplayer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
  
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
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
    
    if ( const FBullet& Indifference = Instance->GetInventory().BulletsStored[EBulletType::Indifference];
        Indifference.IsValid())
    {
        return FText::FromString("x " + FString::FromInt(Indifference.GetQuantity()));
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
    
    const int32 Current = BulletSwitcher->GetActiveWidgetIndex(); 
    const int32 Next = (Current == 0) ? 1 : 0; 
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
    if (CurrentRecipe.Requirements == nullptr) 
    {
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
    
    CraftingTable->CraftSelectedBullet(GetWorld());
    Refresh();
    
    if (!bIsHolding)
    {
        CurrentProgress = 0.0f;
        if (CraftingProgressBar)
        {
            CraftingProgressBar->SetPercent(0.0f);
        }
        return;
    }
  
    if (!CraftingTable->ScanResources(GetWorld()))
    {
        bIsHolding = false;
        CurrentProgress = 0.0f;
        if (CraftingProgressBar)
        {
            CraftingProgressBar->SetPercent(0.0f);
        }
        return;
    }
    
    CurrentProgress = 0.0f;
    if (CraftingProgressBar)
    {
        CraftingProgressBar->SetPercent(0.0f);
    }
    
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

void UInventoryHUD::Bind()
{
    UICC_EnhancedInputCmp* Binder = Cast<UICantCryGameInstance>(GetGameInstance())->GetCurrentPlayer()->GetInputBinder();
    UICC_InputDataAsset* Data = Cast<UICantCryGameInstance>(GetGameInstance())->GetCurrentPlayer()->GetInputDataAsset();
    
    Binder->BindNativeInputAction(Data, Icc_InputTags::InputTag_Interact, ETriggerEvent::Triggered, this, &UInventoryHUD::SimulateCraftClick);
    Binder->BindNativeInputAction(Data, Icc_InputTags::InputTag_Interact, ETriggerEvent::Canceled, this, &UInventoryHUD::EndSimulateCraftClick);
    Binder->BindNativeInputAction(Data, Icc_InputTags::InputTag_ScrollBulletsLeft, ETriggerEvent::Triggered, this, &UInventoryHUD::NavigateLeft);
    Binder->BindNativeInputAction(Data, Icc_InputTags::InputTag_ScrollBulletsRight, ETriggerEvent::Triggered, this, &UInventoryHUD::NavigateRight);
}

void UInventoryHUD::SimulateCraftClick()
{
    OnCraftPressed();
}

void UInventoryHUD::EndSimulateCraftClick()
{
    OnCraftReleased();
}

void UInventoryHUD::NavigateThroughBullets(const int32& InDirection)
{
    if (StandardBulletDisplayer->GetBullet().IsEmpty()) return;

    const int32 OriginalIndex = ScrollerIndex;
    bool bFoundValidButton = false;
	
    for (int32 i = 0; i < StandardBulletDisplayer->GetBullet().Num(); ++i)
    {
        ScrollerIndex = (ScrollerIndex + InDirection + StandardBulletDisplayer->GetBullet().Num()) % StandardBulletDisplayer->GetBullet().Num();

        if (const UBulletBottonItem* Target = StandardBulletDisplayer->GetBullet()[ScrollerIndex])
        {
            if (Target->GetIsEnabled() && Target->GetVisibility() == ESlateVisibility::Visible)
            {
                bFoundValidButton = true;
                break; 
            }
        }
    }
	
    if (bFoundValidButton)
    {
        if (UBulletBottonItem* Target = StandardBulletDisplayer->GetBullet()[ScrollerIndex])
        {
            Target->SetFocus();
            Hightlight(Target);
        }
    }
    else
    {
        ScrollerIndex = OriginalIndex;
    }
}

void UInventoryHUD::NavigateLeft()
{
    NavigateThroughBullets(-1);
    DebugHelper::LogMessage(5, FColor::Silver, "Im supposed to scroll bullet list up");
}

void UInventoryHUD::NavigateRight()
{
    NavigateThroughBullets(1);
    DebugHelper::LogMessage(5, FColor::Silver, "Im supposed to scroll bullet list down");
}

void UInventoryHUD::Hightlight(UWidget* What)
{
    // if (!What || !OverviewFrame) return;
	   //
    // OverviewFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    // UCanvasPanelSlot* FrameSlot = Cast<UCanvasPanelSlot>(OverviewFrame->Slot);
	   //
    // if (const UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(What->Slot);
    //     FrameSlot && ButtonSlot)
    // {
    //     FrameSlot->SetPosition(ButtonSlot->GetPosition());
    //     FrameSlot->SetSize(ButtonSlot->GetSize());
		  //
    //     FrameSlot->SetAnchors(ButtonSlot->GetAnchors());
    //     FrameSlot->SetAlignment(ButtonSlot->GetAlignment());
    // }
}

void UInventoryHUD::RefreshEssence()
{
    EssenceBox->ClearChildren();

    for (const auto &Essence : GameInstance->GetInventory().EssencesStored)
    {
        UEssenceWidget *EssenceWidget = CreateWidget<UEssenceWidget>(GetWorld(), UEssenceWidgetClass);
   
        if (!EssenceWidget)
        {
            DebugHelper::LogMessage(8, FColor::Red,"Essence widget is null");
            return;
        }

        EssenceBox->AddChild(EssenceWidget);
        const FEssence &E = Essence.Value;
        EssenceWidget->Setup(E, E.Quantity);
    }
}

void UInventoryHUD::Setup()
{
    StandardBulletDisplayer->Init(this);
    StandardBulletDisplayer->Refresh();
}





