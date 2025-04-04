// Fill out your copyright notice in the Description page of Project Settings.

#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "EngineUtils.h"
#include "BattleHUD.h"

void UBattleHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Shoot) Shoot->OnClicked.AddDynamic(this, &UBattleHUD::OnShootPressed);
    if (ShootBoost) ShootBoost->OnClicked.AddDynamic(this, &UBattleHUD::OnShootBoostPressed);
    if (Focus) Focus->OnClicked.AddDynamic(this, &UBattleHUD::OnFocusPressed);
    if (Reload) Reload->OnClicked.AddDynamic(this, &UBattleHUD::OnReloadPressed);
    if (Pass) Pass->OnClicked.AddDynamic(this, &UBattleHUD::OnPassPressed);
    if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UBattleHUD::ConfirmBulletSelection);

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
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);


    for (TActorIterator<ABattleHandler> It(GetWorld()); It; ++It)
	{
		BattleHandler = *It;
		break;
	}
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

    if (Enemies.Num() > 0)
    {
        CurrentEnemyIndex = 0;
        UpdateTarget();
    }

    IncreaseAP(1);
    
}

void UBattleHUD::OnShootBoostPressed()
{
    IncreaseAP(2);
}

void UBattleHUD::OnFocusPressed()
{
    DebugHelper::LogWarning("attack and defense increased!");
    IncreaseAP(1);
    //BattleHandler->EndTurn();
}

void UBattleHUD::OnReloadPressed()
{
    IncreaseAP(1);
}

void UBattleHUD::OnPassPressed()
{
    IncreaseAP(1);
}

void UBattleHUD::ScrollTargetSelection(float ScrollValue)
{
    //TArray<AICC_Actor*> Queque = BattleHandler->GetTurnBasedSystem()->GetTurn().Queue;
}

void UBattleHUD::UpdateTarget()
{
    if (Enemies.IsValidIndex(CurrentEnemyIndex))
    {
        AActor* TargetEnemy = Enemies[CurrentEnemyIndex];
        FString EnemyName = TargetEnemy->GetName();
        TargetNameText->SetText(FText::FromString(EnemyName));

       
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
    if (Enemies.IsValidIndex(CurrentEnemyIndex) && Crosshair)
{
    AActor* TargetEnemy = Enemies[CurrentEnemyIndex];
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
    if (!CanvasAmmoSelection || !LoadedBulletData.IsValidIndex(SelectedBulletIndex)) 
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
        UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(BulletIcons[SelectedBulletIndex]->Slot);
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
    if (!LoadedBulletData.IsValidIndex(SelectedBulletIndex)) return;
    if (CurrentRevolverSlot >= RevolverSlots.Num()) return;

    UBulletData* CurrentBullet = LoadedBulletData[SelectedBulletIndex];
    
    // update icon in the revolver slot
    if (RevolverSlots[CurrentRevolverSlot] && CurrentBullet->Icon)
    {
        RevolverSlots[CurrentRevolverSlot]->SetBrushFromTexture(CurrentBullet->Icon);
        RevolverSlots[CurrentRevolverSlot]->SetVisibility(ESlateVisibility::Visible);
        CurrentRevolverSlot++;
    }

    // if the revolver is full, go to battle ui
    if (CurrentRevolverSlot >= MaxRevolverSlots) // CurrentRevolverSlot >= RevolverSlots.Num()
    {
        // Hide ammo selection UI
        if (CanvasAmmoSelection) CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
        if (ConfirmButton) ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
        SwitchToBattleUI();
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

void UBattleHUD::ScrollBulletSelection(int ScrollValue)
{

    if(!BattleHandler) 
    {
        return;
    }

    
    SelectedBulletIndex = (SelectedBulletIndex - 1 + BulletIcons.Num()) % BulletIcons.Num();

    FWidgetTransform Transform;

    Transform.Translation = BulletIcons[SelectedBulletIndex]->GetRenderTransform().Translation;

    Transform.Angle  = AmmoSelectionIndicator->GetRenderTransform().Angle;

    Transform.Scale = AmmoSelectionIndicator->GetRenderTransform().Scale;

    AmmoSelectionIndicator->SetRenderTransform(Transform);

}


