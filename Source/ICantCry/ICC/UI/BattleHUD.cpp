// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleHUD.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/GameOver/GameOverVisualizer.h"
#include "Framework/Application/SlateApplication.h"

void UBattleHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Shoot) Shoot->OnClicked.AddDynamic(this, &UBattleHUD::OnShootPressed);
    if (Focus) Focus->OnClicked.AddDynamic(this, &UBattleHUD::OnFocusPressed);
    if (Reload) Reload->OnClicked.AddDynamic(this, &UBattleHUD::OnReloadPressed);
    if (Pass) Pass->OnClicked.AddDynamic(this, &UBattleHUD::OnPassPressed);
    if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UBattleHUD::ConfirmBulletSelection);
    if (EngageBtn) EngageBtn->OnClicked.AddDynamic(this, &UBattleHUD::Engage);
    ConfirmReloadBullet->OnClicked.AddDynamic(this, &UBattleHUD::UBattleHUD::HideBulletMagazineOnReload);

    //SET VISIBILITY PANNELS

    //HIDE
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::Hidden);
    if (CanvasMainBattlePanel) CanvasMainBattlePanel->SetVisibility(ESlateVisibility::Hidden);
    if (CanvasStatus) CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
    if (ShootBoost) ShootBoost->SetVisibility(ESlateVisibility::Hidden);

    //VISIBLE
    if (CanvasFirstReloadMagazine)  CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Visible);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Visible);
    if (CanvasMiniGames) CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);


    //LOAD BULLET DATA
    LoadedBulletData.Empty();
    for (TSubclassOf<UBulletData> BulletClass : AvailableBulletTypes)
    {
        if (BulletClass)
        {
            LoadedBulletData.Add(NewObject<UBulletData>(this, BulletClass));
        }
    }


    RevolverBuffer = NewObject<UCircularBulletBuffer>(this);

    if (RevolverBuffer)
    {
        RevolverBuffer->Initialize(MaxRevolverSlots);
    }


    // INIT REVOLVER SLOTS
    RevolverSlots = {
        RevolverSlot0,
        RevolverSlot1,
        RevolverSlot2,
        RevolverSlot3,
        RevolverSlot4,
        RevolverSlot5
    };

    PistolMagazines =
    {
        PistolMagazine_1,
        PistolMagazine_2,
        PistolMagazine_3,
        PistolMagazine_4,
        PistolMagazine_5,
        PistolMagazine_6
    };

    MagazineBullets =
    {
        MagazineBullet0,
        MagazineBullet1,
        MagazineBullet2,
        MagazineBullet3,
        MagazineBullet4,
        MagazineBullet5
    };

    for (UMagazineBullet* Bullet : MagazineBullets)
    {
        Bullet->SetRenderOpacity(0.0f);
    }

    ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
    ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

    ApIncreaseOnShoot->OnClicked.AddDynamic(this, &UBattleHUD::IncreaseShootPower);
    ApDecreaseOnShoot->OnClicked.AddDynamic(this, &UBattleHUD::DecreaseShootPower);

    // init all hidden slots before showing
    for (URevolverSlot* RevolverSlot : RevolverSlots)
    {
        if (RevolverSlot)
        {
            RevolverSlot->SetFilled(false);
        }

        RevolverSlot->SetVisibility(ESlateVisibility::Visible);

        DebugHelper::LogError("Called");
    }


    if (!BulletPanel || !BulletIconWidgetClass)
    {
        return;
    }

    // SetSelectedBullet(0);

    //Bullet Selection 
    SelectedBulletIndex = 0;
    CurrentRevolverSlot = 0;
    UpdateBulletSelection();

    // Ammo Selection
    for (UImage* Icon : BulletIcons)
    {
        if (Icon) 
        {
            Icon->SetVisibility(ESlateVisibility::Visible);
            Icon->SetOpacity(0.5f); // reduced opacity for unselected
        }
    }
    
    if (BulletIcons.IsValidIndex(SelectedBulletIndex))
    {
        BulletIcons[SelectedBulletIndex]->SetOpacity(1.0f);
    }
    
    for (TActorIterator<ABattleHandler> It(GetWorld()); It; ++It)
	{
		BattleHandler = *It;
		break;
	}

    for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
    {
        MinigameHandler = *It;
        break;
    }

    CurrentEnemyIndex = 0;

    GameInstance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(this));
    checkf(GameInstance, TEXT("Game instance is null at constructor battle hud"));
    PlayerHealth->SetPercent(GameInstance->GetPlayerStats()->CurrentHealth);

    CurrentEnemyIndex = 0;
    
    RefreshBulletUI();

    CurrentAP = 2;
    UpdateAPBar();

    FTimerHandle TimerHandle;

    GetWorld()->GetTimerManager().SetTimer(
   TimerHandle,                 
   this,                      
   &UBattleHUD::ReflectBullets,  
   0.4f,                            
   false                            
);

    ConfirmReloadBullet->SetVisibility(ESlateVisibility::Hidden);

    //UBulletSelector::SetCanSelect(true);

    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
}

// TARGET
void UBattleHUD::IncreaseAP(const int& Amount)
{
    CurrentAP = FMath::Clamp(CurrentAP + Amount, 0,4);

    UpdateAPBar();
}

void UBattleHUD::DecreaseAP(const int& Amount)
{
    CurrentAP = FMath::Clamp(CurrentAP - Amount, 0,4);
    DebugHelper::LogWarning("AP decreased now " + FString::FromInt(CurrentAP));
    DebugHelper::AddMessageToLog("[BattleHUD]: AP decreased now " + FString::FromInt(CurrentAP));
    UpdateAPBar();
}

void UBattleHUD::UpdateAPBar()
{
    const float Progress = static_cast<float>(CurrentAP) / 4.0f;
    APBar->SetPercent(Progress);
}

void UBattleHUD::OnShootPressed()
{
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);

    if (GameInstance->GetCurrentPlayer()->IsAshamed())
    {
        DebugHelper::LogError("Player can't attack , is in ashamed state!");
        DebugHelper::AddMessageToLog("[BattleHUD]: " + GameInstance->GetCurrentPlayer()->GetActorLabel() + " can't attack because it's under ashamed state!");
        return;
    }
    
    if (!GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn() || CurrentAP <= 0)
    {
        return;
    }

    if (GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->IsFreezed())
    {
        DecreaseAP(1);
        Bar->DecreaseAP(1);
        BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UnfreezeChance();
        return;
    }
    
    const UCircularBulletBuffer* Buffer = GetCircularBulletBuffer();

    if (Buffer->IsEmpty())
    {
        DebugHelper::LogWarning("Buffer is empty");
        BulletName->SetVisibility(ESlateVisibility::Hidden);
        Quantity->SetVisibility(ESlateVisibility::Hidden);
        OutOfBulletTxt->SetVisibility(ESlateVisibility::Visible);
        OutOfBulletTxt->SetColorAndOpacity(FLinearColor::Red);
        OutOfBulletTxt->SetText(FText::FromString("Magazine empty!"));
        return;
    }

    DecisionDisplayer->Hide();
    DisableButtonsDuringShooting();

    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);
    Displayer->SetVisibility(ESlateVisibility::Hidden);
    TargetText->SetVisibility(ESlateVisibility::Visible);
    TargetNameText->SetVisibility(ESlateVisibility::Visible);
    CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
    BulletName->SetVisibility(ESlateVisibility::Hidden);
    Quantity->SetVisibility(ESlateVisibility::Hidden);
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    CanvasStatus->SetVisibility(ESlateVisibility::Visible);
    
    bShootFired = true;
    DebugHelper::LogMessage(7, FColor::Orange, "Ap Accumulator on shoot -> " + FString::FromInt(ApAccumulator));
    Bar->UpdateHighlights(1);
    UpdateAPBar();
    ApAccumulator = 1;
    bTargetSelection = true;
    DebugHelper::LogSuccess("Shoot pressed, ap spent (Accumulator)-> " + FString::FromInt(ApAccumulator));
    DebugHelper::AddMessageToLog("[BattleHUD]: Shoot pressed, ap spent (Accumulator) -> " + FString::FromInt(ApAccumulator));
    
    if (Buffer && !Buffer->IsEmpty())
    {
        const UBulletData* Bullet = Buffer->PeekAt(Buffer->GetTailIndex());
        if (Bullet)
        {
            const EBulletType Type = Bullet->Type;
            
            if (Type == EBulletType::JoyEv ||
                Type == EBulletType::AngerEv ||
                Type == EBulletType::JealousyEv ||
                Type == EBulletType::SadnessEv ||
                Type == EBulletType::CalmEV ||
                Type == EBulletType::FearEV)
            {
                ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
                ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
            }
            else 
            {
                ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Visible);
                ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }
    
    FSlateApplication::Get().ClearAllUserFocus();

}

void UBattleHUD::OnShootBoostPressed()
{
    //IncreaseAP(2);
    // ApAccumulator++;
    // ApAccumulator = (CurrentAP >= 4) ? 4 : ApAccumulator;
    // DebugHelper::LogSuccess("Boosting , ap spent: " + FString::FromInt(ApAccumulator));
}

void UBattleHUD::OnFocusPressed()
{
    if (!GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn())
    {
        return;
    }

    if (GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->IsFreezed())
    {
        IncreaseAP(1);
        return;
    }
    
    DebugHelper::LogWarning("attack and defense increased!");
    DebugHelper::AddMessageToLog("[BattleHUD]: attack and defense increased!");
    IncreaseAP(1);
    Bar->IncreaseAP(1);
    BattleHandler->GetTurnBasedSystem()->EndTurn();
    GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateStatus();
    GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateBuffStatus();
    BattleHandler->GetTurnBasedSystem()->StartNextTurn();
    DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
    BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
    Displayer->SetVisibility(ESlateVisibility::Hidden);
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    bTargetSelection = false;
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
    BulletName->SetVisibility(ESlateVisibility::Hidden);
    Quantity->SetVisibility(ESlateVisibility::Hidden);
    FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::OnReloadPressed()
{
    if (!GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn())
    {
        return;
    }

    if (GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->IsFreezed())
    {
        DecreaseAP(1);
        return;
    }

    //UBulletSelector::SetCanSelect(true);
    
    ConfirmReloadBullet->SetVisibility(ESlateVisibility::Visible);
    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
    DecreaseAP(1);
    UpdateAPBar();
    Bar->DecreaseAP(1);

    bBulletSetupFinished = false;
    bShootFired = false;
    bTargetSelection = false;
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Visible);
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    CanvasBulletStats->SetVisibility(ESlateVisibility::Visible);
    BulletName->SetVisibility(ESlateVisibility::Visible);
    Quantity->SetVisibility(ESlateVisibility::Visible);
    CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
    Displayer->SetVisibility(ESlateVisibility::Visible);
    Displayer->Refresh();
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
    RefreshBulletMagazine();
    FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::OnPassPressed()
{
    if (!BattleHandler->GetTurnBasedSystem()->GetIsPlayerTurn())
    {
        DebugHelper::LogError("You can't pass it's not player turn");
        return;
    }
    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    Displayer->SetVisibility(ESlateVisibility::Hidden);
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    IncreaseAP(1);
    Bar->IncreaseAP(1);
    DebugHelper::LogSuccess("Player passed the turn");
    DebugHelper::AddMessageToLog("[BattleHUD]: Player passed the turn");
    BattleHandler->GetTurnBasedSystem()->EndTurn();
    BattleHandler->GetTurnBasedSystem()->StartNextTurn();
	DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
    BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
    DebugHelper::RemoveOverlayMaterialFromStaticMesh(BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
    bTargetSelection = false;
    BattleHandler->GetBattleInfo()->ClearInfo();
    GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateStatus();
    GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateBuffStatus();
    Displayer->SetVisibility(ESlateVisibility::Hidden);
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
    BulletName->SetVisibility(ESlateVisibility::Hidden);
    Quantity->SetVisibility(ESlateVisibility::Hidden);
    FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::ScrollTargetSelection(float ScrollValue)
{
    TArray<AICC_Actor*> Queue = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue;
    if (Queue.IsEmpty())
    {
        DebugHelper::LogError("Queue is empty at ScrollTargetSelection");
        return;
    }
    AICC_Actor* SelectedActor = Queue[0];
    
    bTargetSelection = true;
    const int QueueSize = Queue.Num();
    const int EnemyCount = QueueSize - 1;
    
    const int Direction = (ScrollValue > 0) ? 1 : (ScrollValue < 0 ? -1 : 0);
    if (Direction == 0 /*|| EnemyCount <= 0*/) return;
    
    CurrentEnemyIndex = (CurrentEnemyIndex + Direction + QueueSize) % QueueSize;
    SelectedActor = Queue[CurrentEnemyIndex];
    
    if (!SelectedActor){ return; }
    
    
    DebugHelper::LogSuccess(FString::FromInt(CurrentEnemyIndex));
    // TargetNameText->SetText(FText::FromString(SelectedActor->GetActorLabel()));
    // TargetText->SetText(FText::FromString(FString(TEXT("Target: ")) + SelectedActor->GetActorLabel()));
    
    DebugHelper::LogMessage(10, FColor::Orange, "Target Selected: " + SelectedActor->GetActorLabel());
    DebugHelper::AddMessageToLog("[BattleHUD]: Target Selected: " + SelectedActor->GetActorLabel());
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
    BulletName->SetVisibility(ESlateVisibility::Hidden);
    Quantity->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleHUD::UpdateTarget()
{
    if (!bTargetSelection)
    {
        return;
    }
    
    if (BattleHandler->GetTurnBasedSystem()->GetTurn().Queue.IsValidIndex(CurrentEnemyIndex))
    {
        ShowInfo();
        bSelectTarget = true;
        AICC_Actor* TargetEnemy = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex];
        AMob* PreviousTargetEnemy = nullptr; 
        bool bOverlayMaterialApplied = false;

        if (GetCircularBulletBuffer()->IsEmpty())
        {
            return;
        }
        
        CurrentBulletData = GetCircularBulletBuffer()->PeekAt(GetCircularBulletBuffer()->GetTailIndex());
        
        if (TargetEnemy->IsA(AICC_Player::StaticClass()) && CurrentBulletData->Type != FearEV && CurrentBulletData->Type != AngerEv &&
            CurrentBulletData->Type != JoyEv)
        {
            HideInfo();
            return;
        }

        if (TargetEnemy->IsA(AMob::StaticClass()) && 
            (CurrentBulletData->Type == FearEV || 
             CurrentBulletData->Type == AngerEv || 
             CurrentBulletData->Type == JoyEv))
        {
            HideInfo();
            return;
        }

        
        AMob* Current = Cast<AMob>(TargetEnemy);

        if (!Current)
        {
            return;
        }
        
        if (PreviousTargetEnemy && PreviousTargetEnemy != Current)
        {
            //DebugHelper::RemoveOverlayMaterialFromStaticMesh(PreviousTargetEnemy->StaticMesh);
            
            bOverlayMaterialApplied = false;
        }
        
        if (!bOverlayMaterialApplied)
        {
            DebugHelper::AddOverlayMaterialToStaticMesh(Current->StaticMesh);
            bOverlayMaterialApplied = true;
            PreviousTargetEnemy = Current;
        }
    }
}

void UBattleHUD::UpdateTargetInfo(const FString& EnemyName, UBulletData* BulletData)
{
    if (!TargetNameText) return;

    FString InfoString = FString::Printf(TEXT("TARGET: %s"), *EnemyName);
    
    if (BulletData)
    {
        InfoString += FString::Printf(TEXT("\n\nBULLET: %s\nPOWER: %d\nEFFECT: %s"), 
            *BulletData->BulletName,
             BulletData->Power,
            *BulletData->Effect);
    }

    TargetNameText->SetText(FText::FromString(InfoString));
}

// BULLET

void UBattleHUD::RefreshBulletUI()
{
    // Ricarica tutte le componenti UI relative ai proiettili
    UpdateBulletIcons(Inventory.GetAllItems());
    SetSelectedBullet(SelectedBulletIndex);
    UpdateRevolverUI();
}

void UBattleHUD::ReflectBullets()
{
    Displayer = CreateWidget<UBulletDisplayer>(GetWorld(), BulletDisplayerClass);
    BulletPanel->AddChild(Displayer);
    SetSelectedBullet(0);
}

void UBattleHUD::UpdateBulletIcons(const TArray<FInventoryItem>& InventoryItems)
{
    if (!BulletPanel || !BulletIconWidgetClass) return;

    BulletPanel->ClearChildren();

    for (const FInventoryItem& Item : InventoryItems)
    {
        if (Item.ItemType == EItemType::Bullet && Item.Bullet.GetBulletData()->Icon)
        {
            UBulletIconWidget* NewIcon = CreateWidget<UBulletIconWidget>(this, BulletIconWidgetClass);
            if (NewIcon)
            {
                NewIcon->SetIcon(Item.Bullet.GetBulletData()->Icon);
                NewIcon->SetQuantity(Item.Quantity);
                BulletPanel->AddChild(NewIcon);
            }
        }
    }
}

UBulletDisplayer* UBattleHUD::GetBulletDisplayer() const
{
    return Displayer;
}


void UBattleHUD::UpdateRevolverUI()
{
    if (!RevolverBuffer)
    {
        return;
    }
       

    // resets the visual state to show only the bullets that are actually present
    for (URevolverSlot* RevolverSlot : RevolverSlots)
    {
        if (RevolverSlot)
        {
            RevolverSlot->SetFilled(false);
        }
    }

    // show only bullet loaded
    for (int32 i = 0; i < RevolverBuffer->GetCount(); i++)
    {
        int32 BufferIndex = (RevolverBuffer->GetTailIndex() + i) % RevolverBuffer->GetCapacity();
        UBulletData* Bullet = RevolverBuffer->PeekAt(BufferIndex);
        
        if (Bullet && Bullet->Icon && RevolverSlots.IsValidIndex(i))
        {
            RevolverSlots[i]->SetFilled(true, Bullet->Icon);
        }
    }
}

void UBattleHUD::HideBulletMagazineOnReload()
{
    ConfirmReloadBullet->SetVisibility(ESlateVisibility::Hidden);
    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    Displayer->SetVisibility(ESlateVisibility::Hidden);
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    //UBulletSelector::SetCanSelect(false);
}

void UBattleHUD::IncreaseShootPower()
{
    if (CurrentAP <= 1)
    {
        return;
    }
    
    ApAccumulator = FMath::Min(ApAccumulator + 1, 4);
    Bar->UpdateHighlights(ApAccumulator);
    
    DebugHelper::LogMessage(6, FColor::Orange, "ApAccumalator on boost pressed " + FString::FromInt(ApAccumulator));
    
    if (ApAccumulator > CurrentAP)
    {
        DebugHelper::LogError("You can't add more ap than you have it current ap " + FString::FromInt(CurrentAP) + "- Accumulator " + FString::FromInt(ApAccumulator));
        DebugHelper::AddMessageToLog("[BattleHUD]: You can't add more ap than you have it current ap " + FString::FromInt(CurrentAP) + "- Accumulator " + FString::FromInt(ApAccumulator));
        ApAccumulator = CurrentAP;
        Bar->UpdateHighlights(ApAccumulator);
    }
    
    
    switch (ApAccumulator)
    {
    case 1:
        {
            GameInstance->GetPlayerStats()->ApModifier = 1.0f;
            DebugHelper::AddMessageToLog("[BattleHUD]: Player spent 1 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            DebugHelper::LogWarning("[BattleHUD]: Player spent 1 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            break;
        }
    case 2:
        {
            GameInstance->GetPlayerStats()->ApModifier = 1.5f;
            DebugHelper::AddMessageToLog("[BattleHUD]: Player spent 2 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            DebugHelper::LogWarning("[BattleHUD]: Player spent 2 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            break;
        }
    case 3:
        {
            GameInstance->GetPlayerStats()->ApModifier = 2.0f;
            DebugHelper::AddMessageToLog("[BattleHUD]: Player spent 3 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            DebugHelper::LogWarning("[BattleHUD]: Player spent 3 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            break;
        }
    case 4:
        {
            GameInstance->GetPlayerStats()->ApModifier = 2.5f;
            DebugHelper::AddMessageToLog("[BattleHUD]: Player spent 4 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            DebugHelper::LogWarning("[BattleHUD]: Player spent 4 extra ap " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            break;
        }
    
    default:
        {
            GameInstance->GetPlayerStats()->ApModifier = 1.0f;
            DebugHelper::AddMessageToLog("[BattleHUD]: Player spent 1 extra ap (default) " + FString::SanitizeFloat(GameInstance->GetPlayerStats()->ApModifier));
            break;
        }
      
    }
    
    Bar->UpdateHighlights(ApAccumulator);
}

void UBattleHUD::DecreaseShootPower()
{
    if (ApAccumulator <= 0)
        return;
    
    ApPowerBoost--;
    ApAccumulator = FMath::Clamp(ApAccumulator - 1, 1, 4);
    
    Bar->UpdateHighlights(ApAccumulator);
    
    DebugHelper::LogMessage(6, FColor::Yellow, 
        "Restored Ap: " + FString::FromInt(ApAccumulator));
    
    
    float NewModifier = 1.0f;
    
    switch (ApAccumulator)
    {
    case 1: NewModifier = 1.0f; break;
    case 2: NewModifier = 1.5f; break;
    case 3: NewModifier = 2.0f; break;
    case 4: NewModifier = 2.5f; break;
    default: NewModifier = 1.0f; break;
    }
    
    GameInstance->GetPlayerStats()->ApModifier = NewModifier;
    DebugHelper::AddMessageToLog("[BattleHUD]: Player decided to decrease boost, now has: " + FString::SanitizeFloat(NewModifier));
    
    Bar->UpdateHighlights(ApAccumulator);
    
    FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::PrepareToEngage()
{
    AMob* SelectedEnemy= Cast<AMob>(BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex]);
    checkf(SelectedEnemy, TEXT("SelectedEnemy is null at UBattleHUD::Engage"));
    SelectedActorTarget = SelectedEnemy;
    SelectedActorTarget = SelectedEnemy;
    FDamage DummyDamage(CurrentBulletData,  GameInstance->GetPlayerStats(), Cast<AMob>(SelectedActorTarget)->GetTactics(),
        Cast<AMob>(SelectedActorTarget)->GetData(), GameInstance);
    GameInstance->SetDamageData(&DummyDamage);
    DebugHelper::LogMessage(3, FColor::White, "Targeting " + SelectedEnemy->GetActorLabel());
    DebugHelper::AddMessageToLog("[BattleHUD]: Targeting " + SelectedEnemy->GetActorLabel() + " using " + CurrentBulletData->BulletName);
    DebugHelper::LogMessage(5, FColor::Emerald, "Minigame modifier -> " + FString::SanitizeFloat(DummyDamage.PlayerStats->MinigameModifier));
    checkf(MinigameHandler, TEXT("Minigame handler is null at UBattleHUD::Engage"));
    MinigameHandler->StartMinigame(true);
    EngageBtn->SetVisibility(ESlateVisibility::Hidden);
    CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
    BulletName->SetVisibility(ESlateVisibility::Hidden);
    Quantity->SetVisibility(ESlateVisibility::Hidden);
}

void UBattleHUD::SpawnVisualizer()
{
    FTimerHandle DelayHudHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHudHandle, [this]()
    {
        TSubclassOf<UVictoryVisualizer> VictoryVisualizerClass = LoadClass<UVictoryVisualizer>(this,TEXT("/Game/ICC/BluePrints/UI/Battle/BP_Victory.BP_Victory_C"));
        checkf(VictoryVisualizerClass, TEXT("VictoryVisualizerClass path invalid"))
        VictoryVisualizer = CreateWidget<UVictoryVisualizer>(GetWorld(), VictoryVisualizerClass);
        checkf(VictoryVisualizer, TEXT("Invalid victory visualizer"))
        VictoryVisualizer->Setup(GetBattleHandler()->GetTurnBasedSystem()->GetTurn().Queue);
        VisualizerSlot->AddChild(VictoryVisualizer);
        VictoryVisualizer->SetVisibility(ESlateVisibility::Hidden);
    }, 5.0f, false);
}

void UBattleHUD::SetSelectedBullet(int32 Index)
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
    if (!Instance || Instance->GetInventory().BulletsStored.IsEmpty())
    {
        return;
    }

    TArray<FBullet> Bullets;
    Instance->GetInventory().BulletsStored.GenerateValueArray(Bullets);

    if (!Displayer)
    {
        
        return;
    }
    
    if (!Displayer->GetBullets().IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid bullet index: %d"), Index);
        return;
    }

    CurrentSelectedBullet = Displayer->GetBullets()[Index];
}

void UBattleHUD::UpdateBulletSelection() 
{
    if (!CanvasAmmoSelection || !LoadedBulletData.IsValidIndex(SelectedBulletIndex) || !bBulletSetupFinished) 
    {
        return;
    }
    
    for (int32 i = 0; i < BulletIcons.Num(); i++)
    {
        if (BulletIcons[i])
        {
            BulletIcons[i]->SetOpacity(i == SelectedBulletIndex ? 1.0f : 0.5f);
            BulletIcons[i]->SetVisibility(ESlateVisibility::Visible);

            // Aggiorna l'icona dal BulletData corrispondente
            if (LoadedBulletData.IsValidIndex(i) && LoadedBulletData[i]->Icon)
            {
                BulletIcons[i]->SetBrushFromTexture(LoadedBulletData[i]->Icon);
            }
        }
    }

    if (AmmoSelectionIndicator && BulletIcons.IsValidIndex(SelectedBulletIndex))
    {
        const UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(BulletIcons[SelectedBulletIndex]->Slot);
        UCanvasPanelSlot* IndicatorSlot = Cast<UCanvasPanelSlot>(AmmoSelectionIndicator->Slot);
        
        if (IconSlot && IndicatorSlot)
        {
            IndicatorSlot->SetPosition(IconSlot->GetPosition());
        }
    }

    // Update bullet info
    UpdateBulletInfo(LoadedBulletData[SelectedBulletIndex]);


    // show the name of the selected bullet in the canvasstatus
    if (TargetNameText_2 && LoadedBulletData[SelectedBulletIndex])
    {
        TargetNameText_2->SetText(FText::FromString(LoadedBulletData[SelectedBulletIndex]->BulletName));
    }
}

void UBattleHUD::UpdateBulletInfo(UBulletData* BulletData)
{
    if (!BulletData || !TargetNameText) return;

    FString InfoString = FString::Printf(TEXT("BULLET: %s\nPOWER: %d\nEFFECT: %s"), 
        *BulletData->BulletName,
        BulletData->Power,
        *BulletData->Effect);

    TargetNameText->SetText(FText::FromString(InfoString));
}

void UBattleHUD::ConfirmBulletSelection() // this is for the confirm button  
{
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
    SwitchToBattleUI();
    bBulletSetupFinished = true;
    bStartFight = true;
    //BattleHandler->GetTurnBasedSystem()->RequestFight(true);
    BattleHandler->GetTurnBasedSystem()->PrePrepareToBattle();
}

void UBattleHUD::SwitchToBattleUI()
{
    if (CanvasFirstReloadMagazine)
        CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    
    if (CanvasMainBattlePanel)
        CanvasMainBattlePanel->SetVisibility(ESlateVisibility::Visible);

    if (CanvasMiniGames)
        CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);

    CurrentAP = 2;
    UpdateAPBar();
}

void UBattleHUD::ScrollBulletSelection(float ScrollValue)
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
    CanvasBulletStats->SetVisibility(ESlateVisibility::Visible);
    BulletName->SetVisibility(ESlateVisibility::Visible);
    Quantity->SetVisibility(ESlateVisibility::Visible);
    
    if (Instance->GetInventory().BulletsStored.IsEmpty()) 
    {
        return;
    }

    TArray<FBullet> Bullets;
    Instance->GetInventory().BulletsStored.GenerateValueArray(Bullets);

    const int Direction = (ScrollValue > 0) ? 1 : (ScrollValue < 0 ? -1 : 0);
    const int32 NewIndex = (SelectedBulletIndex + Direction + Bullets.Num()) % Bullets.Num();

    if (Displayer && Displayer->GetBullets().IsValidIndex(NewIndex))
    {
        CurrentSelectedBullet = Displayer->GetBullets()[NewIndex];
        DebugHelper::LogSuccess("Scrolling " + CurrentSelectedBullet->GetBulletPtr()->GetBulletData()->BulletName);

        CurrentSelectedBullet->DisplayBulletInfo();
        
        FVector2D AbsPos = CurrentSelectedBullet->GetCachedGeometry().GetAbsolutePosition();
        FVector2D LocalPos = AmmoSelectionIndicator->GetParent()->GetCachedGeometry().AbsoluteToLocal(AbsPos);

        if (UCanvasPanelSlot* IndicatorSlot = Cast<UCanvasPanelSlot>(AmmoSelectionIndicator->Slot))
        {
            IndicatorSlot->SetPosition(LocalPos);
        }
    }
    
    SelectedBulletIndex = NewIndex;
}

void UBattleHUD::SetSelectTarget(const bool& Enable)
{
    bSelectTarget = Enable;
}

bool UBattleHUD::GetSelectTarget() const
{
    return bSelectTarget;
}

void UBattleHUD::Engage()
{
    bTargetSelection = false;
    
    if (GetCircularBulletBuffer()->IsEmpty())
    {
        DebugHelper::LogWarning("Revolver is empty can't shoot target");
        return;
    }

    UICantCryGameInstance* PersistentInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    checkf(PersistentInstance, TEXT("Instance is null at void UBattleHUD::UpdateTarget()"));
   
    CurrentBulletData = GetCircularBulletBuffer()->PeekAt(GetCircularBulletBuffer()->GetTailIndex()); // this will take the first avaiable bullet
    checkf(CurrentBulletData, TEXT("Assigned CurrentBulletData invalid"))
    
    switch (CurrentBulletData->Type)
    {
    case Indifference:
        {
            PrepareToEngage();
        }
        break;
    case AngerDv:
        {
            PrepareToEngage();
            break;
        }
    case AngerEv:
        PersistentInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::AtkBuff);
        EngageBtn->SetVisibility(ESlateVisibility::Hidden);
        CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
        GetBulletDisplayer()->RemoveBullet();
        EnableButtonsAfterShooting();
        DecreaseAP(1);
        UpdateAPBar();
        break;
    case FearDv:
        {
            PrepareToEngage();
            break;
        }
    case FearEV:
        EngageBtn->SetVisibility(ESlateVisibility::Hidden);
        CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
        PersistentInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::DefBuff);
        EnableButtonsAfterShooting();
        DecreaseAP(1);
        UpdateAPBar();
        break;
    case Disgust:
        {
            PrepareToEngage();
            break;
        }
    case SadnessDv:
        {
            PrepareToEngage();
            break;
        }
    case JoyDv:
        {
            PrepareToEngage();
            break;
        }
    case JoyEv:
        PersistentInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::LowHealth);
        EngageBtn->SetVisibility(ESlateVisibility::Hidden);
        CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
        GetBulletDisplayer()->RemoveBullet();
        EnableButtonsAfterShooting();
        DecreaseAP(1);
        UpdateAPBar();
        break;
    case Anxiety:
        {
            PrepareToEngage();
            break;
        }
    case CalmDv:
        {
            PrepareToEngage();
            break; 
        }
    case CalmEV:
        {
            // Debuff shield
            EnableButtonsAfterShooting();
            DecreaseAP(1);
            UpdateAPBar();
            GameInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::Shield);
            break;
        }
    case JealousyDv:
        {
            PrepareToEngage();
            break;
        }

    case JealousyEv:
        {
            // Envy Burned
            EnableButtonsAfterShooting();
            DecreaseAP(1);
            UpdateAPBar();
            break;
        }
    case Shame:
        {
            PrepareToEngage();
            GetSelectedActor()->GetStatusTracker()->InflictStatus(EAfflictedStatus::EAShame, Cast<AMob>(GetSelectedActor()));
            break;
        }
    default:
        break;
    }
}

AMob* UBattleHUD::GetCurrentPlayingEmotion() const
{
    return CurrentActiveAI;
}

AMob* UBattleHUD::GetSelectedEmotion() const
{
    return SelectedTarget;
}

AICC_Actor* UBattleHUD::GetSelectedActor() const
{
    return SelectedActorTarget;
}

void UBattleHUD::SetCurrentPlayingEmotion(AMob* Current)
{
    CurrentActiveAI = Current;
    CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
}

UBulletData* UBattleHUD::GetCurrentBulletData() const
{
    return CurrentBulletData;
}

void UBattleHUD::RestoreHealth()
{
    AICC_Player* Player = GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer();

    if (!CurrentBulletData || CurrentBulletData->Type != EBulletType::JoyEv)
    {
        return;
    }
    
    Player->GetStats()->CurrentHealth += CurrentBulletData->Power;
    const float Percentage =  Player->GetStats()->CurrentHealth / Player->GetStats()->MaxHealth;
    PlayerHealth->SetPercent(Percentage);
    DebugHelper::LogWarning("Healed " + FString::SanitizeFloat(Percentage));
}

void UBattleHUD::ResetHealth()
{
    AICC_Player* Player = GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer();
    Player->GetStats()->CurrentHealth = Player->GetStats()->MaxHealth;
    const float Percentage =  Player->GetStats()->CurrentHealth / Player->GetStats()->MaxHealth;
    PlayerHealth->SetPercent(Percentage);
}

void UBattleHUD::DisplayVictoryVisualizer()
{
    VictoryVisualizer->SetVisibility(ESlateVisibility::Visible);
}

void UBattleHUD::SpawnGameOverVisualizer()
{
    FTimerHandle DelayHudHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHudHandle, [this]()
    {
        TSubclassOf<UGameOverVisualizer> GameOverVisualizerClass = LoadClass<UGameOverVisualizer>(this,TEXT("/Game/ICC/BluePrints/UI/Battle/WBP_GameOver.WBP_GameOver_C"));
        checkf(GameOverVisualizerClass, TEXT("GameOverVisualizerClass path invalid"))
        GameOverVisualizer = CreateWidget<UGameOverVisualizer>(GetWorld(), GameOverVisualizerClass);
        checkf(GameOverVisualizer, TEXT("Invalid GameOverVisualizer"))
        VisualizerGameOverSlot->AddChild(GameOverVisualizer);
        GameOverVisualizer->SetVisibility(ESlateVisibility::Hidden);
    }, 5.0f, false);
}

void UBattleHUD::DisplayGameOverVisualizer()
{
    GameOverVisualizer->SetVisibility(ESlateVisibility::Visible);
}

UVictoryVisualizer* UBattleHUD::GetVictoryVisualizer() const
{
    return VictoryVisualizer;
}

void UBattleHUD::SetBulletSetupFinished(const bool& Value)
{
    bBulletSetupFinished = Value;
}

FText UBattleHUD::GetHoveredBulletQuantity()
{
    if (!CurrentSelectedBullet)
    {
        return FText::FromString("0");
    }
	
    return FText::FromString("Quantity: " + FString::FromInt(CurrentSelectedBullet->GetBulletPtr()->GetQuantity()));
}

FText UBattleHUD::UpdateTargetSelectionInfos()
{
    TArray<AICC_Actor*>& Queue = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue;
    
    if (Queue.IsEmpty())
        return FText::FromString("");
    
    // Start at current index and find the first alive target
    for (int32 i = 0; i < Queue.Num(); ++i)
    {
        const int32 CheckIndex = (CurrentEnemyIndex + i) % Queue.Num();
        
        if ( AMob* Mob = Cast<AMob>(Queue[CheckIndex]); !Mob || Mob->IsAlive())
        {
            return FText::FromString("Target: " + Queue[CheckIndex]->GetActorLabel());
        }
    }
    
    return FText::FromString("");
}

FText UBattleHUD::UpdateEnemyName()
{
    TArray<AICC_Actor*>& Queue = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue;

    if (Queue.IsEmpty())
        return FText::FromString("");

    // Start at current index and find the first alive target
    for (int32 i = 0; i < Queue.Num(); ++i)
    {
        const int32 CheckIndex = (CurrentEnemyIndex + i) % Queue.Num();
       
        if ( AMob* Mob = Cast<AMob>(Queue[CheckIndex]); !Mob || Mob->IsAlive())
        {
            return FText::FromString(Queue[CheckIndex]->GetActorLabel());
        }
    }

    return FText::FromString("");
}

void UBattleHUD::UpdateAp()
{
    CurrentAP -= ApAccumulator;
    CurrentAP = FMath::Clamp(CurrentAP, 0, 4);
    UpdateAPBar();
    Bar->SetCurrentAP(CurrentAP);
    Bar->SetPreviewAP(0);
    ApAccumulator = 0;
    Bar->ClearAll();
}

void UBattleHUD::SetApAccumulator(const int& Value)
{
    ApAccumulator = Value;
}

FBullet* UBattleHUD::GetCurrentSelectedBullet() const
{
    return CurrentSelectedBullet->GetBulletPtr();
}

void UBattleHUD::DisableButtonsDuringShooting()
{
    Focus->SetIsEnabled(false);
    Pass->SetIsEnabled(false);
    Reload->SetIsEnabled(false);
    Shoot->SetIsEnabled(false);
}

void UBattleHUD::EnableButtonsAfterShooting()
{
    Focus->SetIsEnabled(true);
    Pass->SetIsEnabled(true);
    Reload->SetIsEnabled(true);
    Shoot->SetIsEnabled(true);
}

void UBattleHUD::RefreshBulletMagazine()
{
    UCircularBulletBuffer* Buffer = GetCircularBulletBuffer();
    const int32 NumSlots = MagazineBullets.Num();

    int32 FilledSlots = 0;

    for (int32 i = 0; i < Buffer->GetCount() && FilledSlots < NumSlots; ++i)
    {
        const int32 BufferIndex =
            (Buffer->GetTailIndex() + i) % Buffer->GetCapacity();

        UBulletData* BulletData = Buffer->PeekAt(BufferIndex);
        if (!BulletData) continue;

        MagazineBullets[FilledSlots]->Setup(Buffer, BulletData, BufferIndex);
        FilledSlots++;
    }
    
    for (int32 i = FilledSlots; i < NumSlots; ++i)
    {
        MagazineBullets[i]->Setup(nullptr, nullptr, -1);
    }
}

void UBattleHUD::ResetBulletMagazine()
{
    const UCircularBulletBuffer* Buffer = GetCircularBulletBuffer();

    for (int32 i = 0; i < Buffer->GetCount(); ++i)
    {
        MagazineBullets[i]->RemoveFromMagazine();
    }
}

void UBattleHUD::ResetAp()
{
    CurrentAP = 2;
    Bar->SetCurrentAP(CurrentAP);
    Bar->SetPreviewAP(0);
    UpdateAPBar();
}

void UBattleHUD::RequestBulletPreparation()
{
    bBulletSetupFinished = false;
    bStartFight = false;

    if (RevolverBuffer)
    {
        RevolverBuffer->Clear();
        RevolverBuffer->Initialize(MaxRevolverSlots);
    }

    for (UMagazineBullet* BulletSlot : MagazineBullets)
    {
        if (!Slot) continue;
        BulletSlot->Setup(nullptr, nullptr, -1);
        BulletSlot->GetMagazineBulletButton()->SetIsEnabled(true);
    }
    
    for (UImage* Img: PistolMagazines)
    {
        Img->SetColorAndOpacity(FColor::White);
        Img->SetBrushFromTexture(nullptr);
    }
    
    ConfirmReloadBullet->SetVisibility(ESlateVisibility::Visible);
    CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
    Displayer->SetVisibility(ESlateVisibility::Visible);
    CanvasAmmoSelection->SetVisibility(ESlateVisibility::Visible);
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    CanvasBulletStats->SetVisibility(ESlateVisibility::Visible);
    BulletName->SetVisibility(ESlateVisibility::Visible);
    Quantity->SetVisibility(ESlateVisibility::Visible);
    CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
    Displayer->SetVisibility(ESlateVisibility::Visible);
    OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);

    DebugHelper::LogWarning("Entered Preparation Phase");

    GetBattleHandler()->GetTurnBasedSystem()->SetBattlePhase(EBattlePhase::Preparation);
}

void UBattleHUD::Reset(const TMap<TEnumAsByte<EBulletType>, FBullet>& Bullets) 
{
    GameInstance->GetInventory().BulletsStored = Bullets;
}

void UBattleHUD::ShowHUD() 
{
    AddToViewport();
    APlayerController* Controller = GetWorld()->GetFirstPlayerController();
    checkf(Controller, TEXT("Controller is null at UBattleHUD::ShowHUD"));

    Controller->bShowMouseCursor = true;
}

bool UBattleHUD::IsShootFired() const
{
    return bShootFired;
}

bool UBattleHUD::IsBulletSelectionOver() const
{
    return bBulletSetupFinished;
}

bool UBattleHUD::IsSelectingTarget() const
{
    return bTargetSelection;
}

void UBattleHUD::SetIsSelectingTarget(const bool& Enable)
{
    bTargetSelection = Enable;
}

void UBattleHUD::ShowInfo() const
{
    TargetText->SetVisibility(ESlateVisibility::Visible);
    TargetNameText->SetVisibility(ESlateVisibility::Visible);
    TargetText_2->SetVisibility(ESlateVisibility::Visible);
    TargetNameText_2->SetVisibility(ESlateVisibility::Visible);
    StatusText->SetVisibility(ESlateVisibility::Visible);
    TargetNameText_1->SetVisibility(ESlateVisibility::Visible);
    TargetText_3->SetVisibility(ESlateVisibility::Visible);
    TargetNameText_3->SetVisibility(ESlateVisibility::Visible);
    EngageBtn->SetVisibility(ESlateVisibility::Visible);
}

void UBattleHUD::HideInfo() const
{
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    TargetText_2->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText_2->SetVisibility(ESlateVisibility::Hidden);
    StatusText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText_1->SetVisibility(ESlateVisibility::Hidden);
    TargetText_3->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText_3->SetVisibility(ESlateVisibility::Hidden);
    EngageBtn->SetVisibility(ESlateVisibility::Hidden);
}

bool UBattleHUD::IsReadyToBattle() const
{
    return bStartFight;
}

ABattleHandler* UBattleHUD::GetBattleHandler() const
{
    return BattleHandler;
}

UCircularBulletBuffer* UBattleHUD::GetCircularBulletBuffer() const
{
    return RevolverBuffer;
}