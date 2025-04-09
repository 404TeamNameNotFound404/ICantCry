// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleHUD.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UBattleHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Shoot) Shoot->OnClicked.AddDynamic(this, &UBattleHUD::OnShootPressed);
    if (ShootBoost) ShootBoost->OnClicked.AddDynamic(this, &UBattleHUD::OnShootBoostPressed);
    if (Focus) Focus->OnClicked.AddDynamic(this, &UBattleHUD::OnFocusPressed);
    if (Reload) Reload->OnClicked.AddDynamic(this, &UBattleHUD::OnReloadPressed);
    if (Pass) Pass->OnClicked.AddDynamic(this, &UBattleHUD::OnPassPressed);
    if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UBattleHUD::ConfirmBulletSelection);
    if (EngageBtn) EngageBtn->OnClicked.AddDynamic(this, &UBattleHUD::Engage);

    //SET VISIBILITY PANNELS

    //HIDE
    TargetText->SetVisibility(ESlateVisibility::Hidden);
    TargetNameText->SetVisibility(ESlateVisibility::Hidden);
    if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::Hidden);
    if (CanvasMainBattlePanel) CanvasMainBattlePanel->SetVisibility(ESlateVisibility::Hidden);
    if (CanvasStatus) CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
    if (ShootBoost) ShootBoost->SetVisibility(ESlateVisibility::Hidden);

    Ammo_1->SetVisibility(ESlateVisibility::Hidden);
    Ammo_2->SetVisibility(ESlateVisibility::Hidden);
    Ammo_3->SetVisibility(ESlateVisibility::Hidden);
    Ammo_4->SetVisibility(ESlateVisibility::Hidden);
    Ammo_5->SetVisibility(ESlateVisibility::Hidden);
    Ammo_6->SetVisibility(ESlateVisibility::Hidden);

    BulletIcons.Add(Bullet_1);
    BulletIcons.Add(Bullet_2);
    BulletIcons.Add(Bullet_3);

    //VISIBLE
    if (CanvasFirstReloadMagazine)  CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Visible);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Visible);
    if (CanvasMiniGames) CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);

    //INIT BULLETS ARRAY
    RevolverSlots = { Ammo_1, Ammo_2, Ammo_3, Ammo_4, Ammo_5, Ammo_6 };

    //LOAD BULLET DATA
    LoadedBulletData.Empty();
    for (TSubclassOf<UBulletData> BulletClass : AvailableBulletTypes)
    {
        if (BulletClass)
        {
            LoadedBulletData.Add(NewObject<UBulletData>(this, BulletClass));
        }
    }

    //INIT BULLET SELECTION
    SelectedBulletIndex = 0;
    CurrentRevolverSlot = 0;
    UpdateBulletSelection();

    // AMMO SELECTION
    for (UImage* Icon : BulletIcons)
    {
        if (Icon) 
        {
            Icon->SetVisibility(ESlateVisibility::Visible);
            Icon->SetOpacity(0.5f); // reduced opacity for unselected
        }
    }
    
    // Evidenzia il proiettile selezionato iniziale
    if (BulletIcons.IsValidIndex(SelectedBulletIndex))
    {
        BulletIcons[SelectedBulletIndex]->SetOpacity(1.0f);
    }

    //FIND ENEMY IN THE LEVEL
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies); // Questo non credo serva piu visto che Enemies è vuoto


    for (TActorIterator<ABattleHandler> It(GetWorld()); It; ++It)
	{
		BattleHandler = *It;
		break;
	}

    DebugHelper::LogError("Lenght bullet icon " + FString::FromInt(BulletIcons.Num()));

    for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
    {
        MinigameHandler = *It;
        break;
    }

    CurrentEnemyIndex = 0;
}



// TARGET
void UBattleHUD::IncreaseAP(int Amount)
{
    CurrentAP = FMath::Clamp(CurrentAP + Amount, 0,4);

    UpdateAPBar();
}

void UBattleHUD::UpdateAPBar()
{
    if(APBar)
    {
        float Progress = static_cast<float>(CurrentAP) / 4.0f;
        APBar->SetPercent(Progress);     
    }
}

void UBattleHUD::OnShootPressed()
{
    TargetText->SetVisibility(ESlateVisibility::Visible);
    TargetNameText->SetVisibility(ESlateVisibility::Visible);
    bShootFired = true;
    IncreaseAP(1);
    bTargetSelection = true;
    DebugHelper::LogSuccess("Shoot pressed");
}

void UBattleHUD::OnShootBoostPressed()
{
    IncreaseAP(2);
}

void UBattleHUD::OnFocusPressed()
{
    DebugHelper::LogWarning("attack and defense increased!");
    IncreaseAP(1);
    BattleHandler->GetTurnBasedSystem()->EndTurn();
    BattleHandler->GetTurnBasedSystem()->StartNextTurn();
    DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
    BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
    bTargetSelection = false;
}

void UBattleHUD::OnReloadPressed()
{
    IncreaseAP(1);
}

void UBattleHUD::OnPassPressed()
{
    if (!BattleHandler->GetTurnBasedSystem()->GetIsPlayerTurn())
    {
        DebugHelper::LogError("You can't pass it's not player turn");
        return;
    }
    
    IncreaseAP(1);
    DebugHelper::LogSuccess("Player passed the turn");
    BattleHandler->GetTurnBasedSystem()->EndTurn();
    BattleHandler->GetTurnBasedSystem()->StartNextTurn();
    DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
    BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
    bTargetSelection = false;
}

void UBattleHUD::ScrollTargetSelection(float ScrollValue)
{
    bTargetSelection = true;
    TArray<AICC_Actor*> Queue = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue;

    if (Queue.IsEmpty())
    {
        DebugHelper::LogError("Queue is empty at ScrollTargetSelection");
        return;
    }
  
    const int QueueSize = Queue.Num();
    const int EnemyCount = QueueSize - 1;

    const int Direction = (ScrollValue > 0) ? 1 : (ScrollValue < 0 ? -1 : 0);
    if (Direction == 0 /*|| EnemyCount <= 0*/) return;

    CurrentEnemyIndex = (CurrentEnemyIndex + Direction + QueueSize) % QueueSize;
    DebugHelper::LogSuccess(FString::FromInt(CurrentEnemyIndex));
    TargetNameText->SetText(FText::FromString(Queue[CurrentEnemyIndex]->GetActorLabel()));
    TargetText->SetText(FText::FromString(FString(TEXT("Target: ")) + Queue[CurrentEnemyIndex]->GetActorLabel()));
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
        static AMob* PreviousTargetEnemy = nullptr; 
        static bool bOverlayMaterialApplied = false;
        
        if (TargetEnemy->IsA(AICC_Player::StaticClass())) // if TargetEnemy is Player just skip it , we don't need to target ourselves right? and we all the info will be hidden for now 
        {
            HideInfo();
            return;
        }
        
        AMob* Current = Cast<AMob>(TargetEnemy);
       
        if (PreviousTargetEnemy && PreviousTargetEnemy != Current)
        {
            DebugHelper::RemoveOverlayMaterialFromStaticMesh(PreviousTargetEnemy->StaticMesh);
            
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

void UBattleHUD::UpdateCrosshair()
{
    if (BattleHandler->GetTurnBasedSystem()->GetTurn().Queue.IsValidIndex(CurrentEnemyIndex) && Crosshair)
{
    AActor* TargetEnemy = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex];
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    FVector2D ScreenPosition;
    if (PC->ProjectWorldLocationToScreen(TargetEnemy->GetActorLocation(), ScreenPosition))
    {
        // Assicurati che Crosshair sia un widget Canvas
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot);
        if (CanvasSlot)
        {
            CanvasSlot->SetPosition(ScreenPosition);  // Imposta la posizione correttamente
            Crosshair->SetVisibility(ESlateVisibility::Visible);
        }
    }
}
}

// BULLET
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
    /*
     * Commento tutto questo perche non mi fa andare avanti , array dei bullet non funziona ,
     * la selezione del index del bullet non funziona perche hai scritto una formula dove moltiplichi il modulo per 0 (perche array di bulleticon è vuoto)
     * io intanto lo commento il resto che vedi sotto non commentato non lo toccare se no non parte la fight poi
     */
    // if (!LoadedBulletData.IsValidIndex(SelectedBulletIndex))
    // {
    //     DebugHelper::LogError("!LoadedBulletData.IsValidIndex(SelectedBulletIndex)");
    //     return;
    // }
    //
    // if (CurrentRevolverSlot >= RevolverSlots.Num())
    // {
    //     DebugHelper::LogError("CurrentRevolverSlot >= RevolverSlots.Num()");
    //     return;
    // }
    //
    // const UBulletData* CurrentBullet = LoadedBulletData[SelectedBulletIndex];
    //
    // // update icon in the revolver slot
    // if (RevolverSlots[CurrentRevolverSlot] && CurrentBullet->Icon)
    // {
    //     RevolverSlots[CurrentRevolverSlot]->SetBrushFromTexture(CurrentBullet->Icon);
    //     RevolverSlots[CurrentRevolverSlot]->SetVisibility(ESlateVisibility::Visible);
    //     CurrentRevolverSlot++;
    // }
    //
    // // if the revolver is full, go to battle ui
    // if (CurrentRevolverSlot >= MaxRevolverSlots) // CurrentRevolverSlot >= RevolverSlots.Num()
    // {
    //     // Hide ammo selection UI
    //     if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    //     if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
    //     SwitchToBattleUI();
    // }
    
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
    SwitchToBattleUI();
    bBulletSetupFinished = true;
    bStartFight = true;
    BattleHandler->GetTurnBasedSystem()->RequestFight(true);
}

void UBattleHUD::SwitchToBattleUI()
{
    if (CanvasFirstReloadMagazine)
        CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
    
    if (CanvasMainBattlePanel)
        CanvasMainBattlePanel->SetVisibility(ESlateVisibility::Visible);

    if (CanvasMiniGames)
        CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);

    CurrentAP = 0;
    UpdateAPBar();
}

void UBattleHUD::ScrollBulletSelection(int ScrollValue)
{
    // if(!BattleHandler) 
    // {
    //     return;
    // }

    if (FMath::IsNearlyZero(static_cast<float>(ScrollValue)))
    {
        UE_LOG(LogTemp, Warning, TEXT("Scroll value is zero, skipping scroll"));
        return;
    }
    
    const int32 Direction = (ScrollValue > 0) ? 1 : -1;
    SelectedBulletIndex = (SelectedBulletIndex + Direction + BulletIcons.Num()) % BulletIcons.Num(); 

    FWidgetTransform Transform;

    Transform.Translation = BulletIcons[SelectedBulletIndex]->GetRenderTransform().Translation;

    Transform.Angle  = AmmoSelectionIndicator->GetRenderTransform().Angle;

    Transform.Scale = AmmoSelectionIndicator->GetRenderTransform().Scale;

    AmmoSelectionIndicator->SetRenderTransform(Transform);

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
    UICantCryGameInstance* PersistentInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    checkf(PersistentInstance, TEXT("Instance is null at void UBattleHUD::UpdateTarget()"));
    AMob* SelectedEnemy = Cast<AMob>(BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex]);
    checkf(SelectedEnemy, TEXT("SelectedEnemy is null at UBattleHUD::Engage"));
    Damage.BulletData = CurrentBulletData; // Current Bullet data is null , must be defined the array of bullet data first because it's empty right now
    Damage.EnemyData = SelectedEnemy->GetData();
    Damage.AIMoves = SelectedEnemy->GetTactics();
    Damage.PlayerStats = PersistentInstance->GetPlayerStats();
    PersistentInstance->SetDamageData(Damage);
    PersistentInstance->GetCurrentDamageData().CalculateDamage(true);
    DebugHelper::LogMessage(3, FColor::White, "Targeting " + SelectedEnemy->GetActorLabel());
    checkf(MinigameHandler, TEXT("Minigame handler is null at UBattleHUD::Engage"));
    MinigameHandler->StartMinigame(true);
    EngageBtn->SetVisibility(ESlateVisibility::Hidden);
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


