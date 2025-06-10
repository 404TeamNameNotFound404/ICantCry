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
    if (CanvasBulletStats) CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
    if (ShootBoost) ShootBoost->SetVisibility(ESlateVisibility::Hidden);

    //VISIBLE
    if (CanvasFirstReloadMagazine)  CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Visible);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Visible);
    if (CanvasMiniGames) CanvasMiniGames->SetVisibility(ESlateVisibility::Visible);


    //LOAD BULLET DATA
    LoadedBulletData.Empty();
    for (const TSubclassOf<UBulletData>& BulletClass : AvailableBulletTypes)
    {
         if (BulletClass)
        {
            if (UBulletData* BulletData = NewObject<UBulletData>(this, BulletClass))
            {
                LoadedBulletData.Add(BulletData);
            }
        }
    }

    //INIT BULLET SELECTION
    SelectedBulletIndex = 0;
    CurrentRevolverSlot = 0;
    UpdateBulletSelection();


    // Init CircularBuffer
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

    // init all hidden slots before showing
    for (URevolverSlot* RevolverSlot : RevolverSlots)
    {
        if (RevolverSlot)
        {
            RevolverSlot->SetFilled(false);
        }
    }


    if (!BulletPanel || !BulletIconWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BattleHUD initialization failed - missing required widgets"));
        return;
    }


    
    // highlight the initial selected bullet
    SetSelectedBullet(SelectedBulletIndex);
   

    //FIND ENEMY IN THE LEVEL
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies); // Questo non credo serva piu visto che Enemies è vuoto


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

    RefreshBulletUI();

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

// BULLET

void UBattleHUD::RefreshBulletUI()
{
    // Ricarica tutte le componenti UI relative ai proiettili
    UpdateBulletIcons(Inventory.GetAllItems());
    SetSelectedBullet(SelectedBulletIndex);
    UpdateRevolverUI();
}

void UBattleHUD::SetSelectedBullet(int32 Index)
{
    if (!BulletPanel || !LoadedBulletData.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid bullet selection"));
        return;
    }
        

    // Aggiorna l'indice selezionato
    SelectedBulletIndex = Index;

    // Highlight delle icone
    TArray<UWidget*> Children = BulletPanel->GetAllChildren();

    for (int32 i = 0; i < Children.Num(); ++i)
    {
        if (UBulletIconWidget* IconWidget = Cast<UBulletIconWidget>(Children[i]))
        {
            const bool bIsSelected = (i == Index);
            
            // 5. Imposta l'highlight
            IconWidget->SetRenderOpacity(bIsSelected ? 1.0f : 0.5f);

            // 6. Gestione indicatore di selezione
            if (bIsSelected && AmmoSelectionIndicator)
            {
                if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
                {
                    if (UCanvasPanelSlot* IndicatorSlot = Cast<UCanvasPanelSlot>(AmmoSelectionIndicator->Slot))
                    {
                        // 7. Animazione smooth
                        const FVector2D TargetPosition = IconSlot->GetPosition();
                        const FVector2D TargetAlignment = IconSlot->GetAlignment();
                        
                        if (bAnimateSelection)
                        {
                            // Calcola la nuova posizione con interpolazione
                            const FVector2D CurrentPosition = IndicatorSlot->GetPosition();
                            const float DeltaTime = GetWorld()->GetDeltaSeconds();
                            const FVector2D NewPosition = FMath::Vector2DInterpTo(
                                CurrentPosition,
                                TargetPosition,
                                DeltaTime,
                                SelectionAnimSpeed
                            );
                            
                            IndicatorSlot->SetPosition(NewPosition);
                        }
                        else
                        {
                            // Imposta direttamente la posizione
                            IndicatorSlot->SetPosition(TargetPosition);
                        }
                        
                        // Allineamento sempre immediato
                        IndicatorSlot->SetAlignment(TargetAlignment);
                    }
                }
            }
        }
    }

    // 8. Aggiornamento dati proiettile
    UpdateBulletStats(LoadedBulletData[Index]);
    
    // 9. Aggiornamento nome proiettile
    if (TargetNameText_2)
    {
        TargetNameText_2->SetText(FText::FromString(LoadedBulletData[Index]->BulletName));
    }

}

void UBattleHUD::UpdateBulletSelection() 
{
    if (!CanvasAmmoSelection || !BulletPanel || !bBulletSetupFinished) 
    {
        return;
    }

    // Ottieni tutti i widget figli del BulletPanel
    TArray<UWidget*> Children = BulletPanel->GetAllChildren();
    
    // Aggiorna l'opacità e lo stato di ogni icona
    for (int32 i = 0; i < Children.Num(); i++)
    {
        UBulletIconWidget* BulletIconWidget = Cast<UBulletIconWidget>(Children[i]);
        if (BulletIconWidget)
        {
            // Imposta l'opacità in base alla selezione
            BulletIconWidget->SetRenderOpacity(i == SelectedBulletIndex ? 1.0f : 0.5f);
            
            // Aggiorna l'indicatore di selezione se esiste
            if (AmmoSelectionIndicator && i == SelectedBulletIndex)
            {
                if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(BulletIconWidget->Slot))
                {
                    UCanvasPanelSlot* IndicatorSlot = Cast<UCanvasPanelSlot>(AmmoSelectionIndicator->Slot);
                    if (IndicatorSlot)
                    {
                        IndicatorSlot->SetPosition(IconSlot->GetPosition());
                    }
                }
            }
        }
    }

    // Aggiorna le informazioni del proiettile selezionato
    if (LoadedBulletData.IsValidIndex(SelectedBulletIndex))
    {
        UpdateBulletStats(LoadedBulletData[SelectedBulletIndex]);
        UpdateBulletInfo(LoadedBulletData[SelectedBulletIndex]);
    }

    // Mostra il nome del proiettile selezionato
    if (TargetNameText_2 && LoadedBulletData.IsValidIndex(SelectedBulletIndex))
    {
        TargetNameText_2->SetText(FText::FromString(LoadedBulletData[SelectedBulletIndex]->BulletName));
    }
}

void UBattleHUD::UpdateBulletInfo(UBulletData* BulletData)
{
    if (!BulletData || !TargetNameText_2)
        return;

    TargetNameText_2->SetText(FText::FromString(BulletData->BulletName));
}



void UBattleHUD::UpdateBulletStats(UBulletData *BulletData)
{
    if (!BulletData || !CanvasBulletStats) 
    {
        return;
    }

    CanvasBulletStats->SetVisibility(ESlateVisibility::Visible);

    if (BulletName) 
    {
        BulletName->SetText(FText::FromString(BulletData->BulletName));
    }

    BulletQuantity = Inventory.GetBulletQuantity(BulletData);

    if (QuantityTotal)
    {
        QuantityTotal->SetText(FText::AsNumber(BulletQuantity));
    }
        


    if (Description) 
    {
        FString Desc = FString::Printf(TEXT("Power: %d\nEffect: %s"),
        BulletData->Power, *BulletData->Effect);
        Description->SetText(FText::FromString(Desc));
    }
}

void UBattleHUD::UpdateBulletIcons(const TArray<FInventoryItem> &InventoryItems)
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


void UBattleHUD::ConfirmBulletSelection() // this is for the confirm button  
{
    if (!CanvasAmmoSelection->IsVisible() || RevolverBuffer->IsFull())
    {
        return;
    }
    

    if (!LoadedBulletData.IsValidIndex(SelectedBulletIndex))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid bullet selection index"));
        return;
    }

    UBulletData* SelectedBullet = LoadedBulletData[SelectedBulletIndex];
    if (Inventory.GetBulletQuantity(SelectedBullet) <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No bullets left to add"));
        return;
    }

    // Aggiunge al revolver e rimuove dall'inventario
    RevolverBuffer->AddBullet(SelectedBullet);
    Inventory.RemoveBullet(SelectedBullet, 1);

    // Aggiorna l'UI
    UpdateRevolverUI();
    SetSelectedBullet(SelectedBulletIndex);

    // Se il revolver è pieno, passa alla fase di battaglia
    if (RevolverBuffer->IsFull())
    {
        SwitchToBattlePhase();
    }

    
    
    
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

void UBattleHUD::SwitchToBattlePhase()
{
    if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
    if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
    if (CanvasBulletStats) CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
    
    SwitchToBattleUI();
    bBulletSetupFinished = true;
    bStartFight = true;
    BattleHandler->GetTurnBasedSystem()->RequestFight(true);
}

void UBattleHUD::CleanRef()
{
    RevolverBuffer = nullptr;
    BattleHandler = nullptr;
      
    LoadedBulletData.Empty();
}

void UBattleHUD::ScrollBulletSelection(float ScrollValue)
{
    // if(!BattleHandler) 
    // {
    //     return;
    // }

    if (LoadedBulletData.Num() == 0) 
    {
        return;
    }

    const int Direction = (ScrollValue > 0) ? 1 : (ScrollValue < 0 ? -1 : 0);
    const int32 NewIndex = (SelectedBulletIndex + Direction + LoadedBulletData.Num()) % LoadedBulletData.Num();

    DebugHelper::LogWarning("Selected index: " + FString::FromInt(SelectedBulletIndex));

    // 3. Aggiornamento stato solo se cambia la selezione
    if (NewIndex != SelectedBulletIndex)
    {
        SelectedBulletIndex = NewIndex;
        
        // 4. Aggiornamento UI (versione ottimizzata)
        SetSelectedBullet(SelectedBulletIndex);
        
        // 5. Log di debug
        UE_LOG(LogTemp, Verbose, TEXT("Bullet selection changed to index: %d"), SelectedBulletIndex);
    }


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


