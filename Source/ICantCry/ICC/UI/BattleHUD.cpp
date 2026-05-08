// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleHUD.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "EngineUtils.h"
#include "HeadMountedDisplayTypes.h"
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

	// Gamepad stuff initializations
	Shoot->IsFocusable = true;
	Focus->IsFocusable = true;
	Reload->IsFocusable = true;
	Pass->IsFocusable = true;
	ConfirmButton->IsFocusable = true;
	EngageBtn->IsFocusable = true;
	ConfirmReloadBullet->IsFocusable = true;

	ActionPhaseButtons = {Shoot, Focus, Reload, Pass};
	BulletPhaseButtons = {ConfirmButton, ConfirmReloadBullet};

	//SET VISIBILITY PANNELS

	//HIDE
	TargetText->SetVisibility(ESlateVisibility::Hidden);
	TargetNameText->SetVisibility(ESlateVisibility::Hidden);
	if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::Hidden);
	if (CanvasMainBattlePanel) CanvasMainBattlePanel->SetVisibility(ESlateVisibility::Hidden);
	if (CanvasStatus) CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
	if (ShootBoost) ShootBoost->SetVisibility(ESlateVisibility::Hidden);

	//VISIBLE
	if (CanvasFirstReloadMagazine) CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Visible);
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

	for (int32 i = 0; i < MagazineBullets.Num(); ++i)
	{
		MagazineBullets[i]->SetRenderOpacity(0.0f);
		MagazineBullets[i]->SetBufferIndex(i);
		MagazineBullets[i]->SetIsFocusable(false);
	}
	

	for (UImage* Img : PistolMagazines)
	{
		Img->SetColorAndOpacity(FColor::White);
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
	}


	if (!BulletPanel || !BulletIconWidgetClass)
	{
		return;
	}

	SetSelectedBullet(0);

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
	PlayerHealth->SetPercent(GameInstance->GetRuntimeStats().CurrentHealth);

	CurrentEnemyIndex = 0;

	RefreshBulletUI();

	CurrentAP = 2;
	UpdateAPBar();

	AmmoSelectionIndicator->SetVisibility(ESlateVisibility::Hidden);

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
	ApAccumulator = 0;
	
	CTR->SetVisibility(ESlateVisibility::Hidden);
	
	const float Percent = GameInstance->GetRuntimeStats().CurrentHealth / GameInstance->GetPlayerStats()->MaxHealth;
	PlayerHealth->SetPercent(Percent);
}

// TARGET
void UBattleHUD::IncreaseAP(const int& Amount)
{
	CurrentAP = FMath::Clamp(CurrentAP + Amount, 0, 4);

	UpdateAPBar();
}

void UBattleHUD::DecreaseAP(const int& Amount)
{
	CurrentAP = FMath::Clamp(CurrentAP - Amount, 0, 4);
	DebugHelper::LogWarning("AP decreased now " + FString::FromInt(CurrentAP));
	DebugHelper::AddMessageToLog("[BattleHUD]: AP decreased now " + FString::FromInt(CurrentAP));
	UpdateAPBar();
}

void UBattleHUD::ProcessExp(AMob* DeathEmotion)
{
	if (!DeathEmotion)
	{
		return;
	}
	
	GameInstance->GetRuntimeStats().ExpSummary += DeathEmotion->GetData()->ExpGiven;
	
	GameInstance->GetRuntimeStats().Experience += DeathEmotion->GetData()->ExpGiven;
	
	DebugHelper::AddMessageToLog("[BattleHud]: " + DeathEmotion->GetActorLabel() +" is death so processing exp.. "
		"Exp given " + FString::SanitizeFloat(DeathEmotion->GetData()->ExpGiven) + " so player exp is: " +
		FString::SanitizeFloat(GameInstance->GetRuntimeStats().Experience));
	
	if (GameInstance->GetRuntimeStats().Experience >= GameInstance->GetRuntimeStats().MaxExperience)
	{
		DebugHelper::LogSuccess("Level Up!");
		DebugHelper::AddMessageToLog("[BattleHud]: Level Up!");
		// for now 
		GameInstance->GetRuntimeStats().MaxExperience += 100;
	}
}

void UBattleHUD::RetrieveNotUsedBullets()
{
	for (const auto& Bullet: GetCircularBulletBuffer()->GetBulletsLeft())
	{
		DebugHelper::AddMessageToLog("[BattleHud]: Bullets left in magazine after battle: " + Bullet->BulletName);
		
		if (!Bullet)
		{
			DebugHelper::AddMessageToLog("[BattleHud]: Cant find the proper bullet inside the map , can't put bullets back");
			continue;
		}
		
		const EBulletType Type = Bullet->Type;
		
		if (FBullet* Existing = GameInstance->GetInventory().BulletsStored.Find(Type); Existing)
		{
			const int32 NewQuantity = Existing->GetQuantity() + 1;
			Existing->SetQuantity(NewQuantity);
			DebugHelper::LogMessage(5, FColor::White, "Putting back " + Existing->GetBulletData()->BulletName + " quantity " + 
				FString::FromInt(Existing->GetQuantity()));
			
			DebugHelper::AddMessageToLog("[BattleHud]: Putting back " + Existing->GetBulletData()->BulletName + " quantity " + 
				FString::FromInt(Existing->GetQuantity()));
		}
		else
		{
			FBullet NewBullet;
			NewBullet.SetBulletData(Bullet);
			NewBullet.SetQuantity(1);
			GameInstance->GetInventory().BulletsStored.Add(Type, NewBullet);
			
			DebugHelper::LogMessage(5, FColor::White, "Creating and Putting back " + NewBullet.GetBulletData()->BulletName + " quantity " + 
				FString::FromInt(NewBullet.GetQuantity()));
			
			DebugHelper::AddMessageToLog("[BattleHud]: Creating and Putting back " + NewBullet.GetBulletData()->BulletName + " quantity " + 
				FString::FromInt(NewBullet.GetQuantity()));
		}
	}
}

void UBattleHUD::PushBackIndifferenceAsCasing(const int32& CasingQuantity, const ECasingType& CasingType)
{
	const FName TargetCasingName = "Base";
	
	
	if (const FCasing* TargetCasing = GameInstance->GetCasingsTable()->FindRow<FCasing>(TargetCasingName, TEXT("Looking for")); 
	   TargetCasing)
	{
		FCasing Casing = *TargetCasing;
		Casing.SetQuantity(CasingQuantity + GameInstance->GetInventory().CasingsStored["Base"].GetQuantity());
		Casing.SetType(CasingType);
		GameInstance->GetInventory().CasingsStored.Add(Casing.GetName(), Casing);
	}
	
}

FText UBattleHUD::UpdateBulletName()
{
	if (!GetCurrentBulletData()) return FText::FromString("");
	
	return FText::FromString(GetCurrentBulletData()->BulletName);
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
		DebugHelper::AddMessageToLog(
			"[BattleHUD]: " + GameInstance->GetCurrentPlayer()->GetActorLabel() +
			" can't attack because it's under ashamed state!");
		
		Shoot->SetIsEnabled(false);
		return;
	}
	
	Shoot->SetIsEnabled(true);

	if (!GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn() /* || CurrentAP <= 0*/)
	{
		return;
	}
	
	if (CurrentAP <= 0)
	{
		DebugHelper::LogMessage(8, FColor::Red, "Ap 0");
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
	DecisionDisplayer->SetDecisionText(FText::FromString(""));
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
	DebugHelper::AddMessageToLog(
		"[BattleHUD]: Shoot pressed, ap spent (Accumulator) -> " + FString::FromInt(ApAccumulator));

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
	bTargetSelection = true;

	bIsEvFirst = false;
	//FSlateApplication::Get().ClearAllUserFocus();
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
	
	// GameInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::DefBuff);
	IncreaseAP(1);
	Bar->IncreaseAP(1);
	GameInstance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	
	/*
	 * TODO Reorder bullets in magazine allowing player to change slot idx ( show confirm button and bullet magazine hud ) change only the bullets in magazine , NO ADD
	 */
	
	BattleHandler->GetTurnBasedSystem()->EndTurn();
	GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateStatus();
	GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateBuffStatus();
	FTimerHandle StartNextHandle;

	GetWorld()->GetTimerManager().SetTimer(StartNextHandle, [this]()
	{
		BattleHandler->GetTurnBasedSystem()->StartNextTurn();
	}, 0.35f, false);

	DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(
		BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
	BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
	Displayer->SetVisibility(ESlateVisibility::Hidden);
	CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
	CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
	bTargetSelection = false;
	OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	Quantity->SetVisibility(ESlateVisibility::Hidden);
	bIsEvFirst = false;
	FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::OnReloadPressed()
{
	bBulletSetupFinished = false;
	bShootFired = false;
	bTargetSelection = false;
	bSelectTarget = false;
	GameInstance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();

	if (GameInstance->GetInventory().BulletsStored.IsEmpty())
	{
		OutOfBulletTxt->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn())
	{
		DebugHelper::AddMessageToLog("[BattleHUD]: Player clicked reload to reload but Detective is freezed");
		return;
	}

	if (GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->IsFreezed())
	{
		DecreaseAP(1);
		DebugHelper::AddMessageToLog("[BattleHUD]: Attempted to reload but Detective is freezed");
		return;
	}

	AmmoSelectionIndicator->SetVisibility(GetBattleHandler()->IsControllerPlugged()
		                                      ? ESlateVisibility::Visible
		                                      : ESlateVisibility::Hidden);

	Displayer->GetBulletConfirmGamepad()->SetVisibility(DebugHelper::IsGamepadPlugged() ?
		ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
	DecreaseAP(1);
	UpdateAPBar();
	Bar->DecreaseAP(1);

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

	Displayer->Refresh();
	RefreshPistolMagazine();
	OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
	Description->SetVisibility(ESlateVisibility::Visible);
	GameInstance->GetCurrentPlayer()->GetBinder()->SetIsNavigatingInsideWidget(true);
	GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(Displayer->GetBulletGrid());
	bIsEvFirst = false;
	//FSlateApplication::Get().ClearAllUserFocus();
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
	// IncreaseAP(1);
	// Bar->IncreaseAP(1);
	DebugHelper::LogSuccess("Player passed the turn");
	DebugHelper::AddMessageToLog("[BattleHUD]: Player passed the turn");
	BattleHandler->GetTurnBasedSystem()->EndTurn();
	FTimerHandle StartNextHandle;

	GetWorld()->GetTimerManager().ClearTimer(StartNextHandle);
	GetWorld()->GetTimerManager().SetTimer(StartNextHandle, [this]()
	{
		IncreaseAP(1);
		Bar->IncreaseAP(1);
		BattleHandler->GetTurnBasedSystem()->StartNextTurn();
	}, 0.35f, false);

	DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(
		BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
	BattleHandler->GetTurnBasedSystem()->SetTurnOverlayApplied(false);
	DebugHelper::RemoveOverlayMaterialFromStaticMesh(
		BattleHandler->GetTurnBasedSystem()->TryGetCurrentPlayer()->DebugMesh);
	bTargetSelection = false;
	
	GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateStatus();
	GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateBuffStatus();
	GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetStatusTracker()->UpdateDebuffStatus();
	Displayer->SetVisibility(ESlateVisibility::Hidden);
	OutOfBulletTxt->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	Quantity->SetVisibility(ESlateVisibility::Hidden);
	FSlateApplication::Get().ClearAllUserFocus();
	bIsEvFirst = false;
}

void UBattleHUD::ScrollTargetSelection(float ScrollValue)
{
	if (!bTargetSelection)
	{
		return;
	}
	
	if (!BattleHandler || !BattleHandler->GetTurnBasedSystem() || (!BattleHandler->GetTurnBasedSystem()->GetIsPlayerTurn()))
	{
		return;
	}
	
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

	if (!SelectedActor) { return; }


	DebugHelper::LogSuccess(FString::FromInt(CurrentEnemyIndex));
	// TargetNameText->SetText(FText::FromString(SelectedActor->GetActorLabel()));
	// TargetText->SetText(FText::FromString(FString(TEXT("Target: ")) + SelectedActor->GetActorLabel()));

	TargetNameText->SetText(FText::FromString(SelectedActor->GetActorLabel()));
	TargetText->SetText(FText::FromString(FString(TEXT("Target: ")) + SelectedActor->GetActorLabel()));
	
	if (!bBulletSetupFinished)
	{
		TargetNameText_2->SetText(FText::FromString(CurrentBulletData->BulletName));
	}
	
	TargetNameText_1->SetText(FText::FromString("None")); // STATUS
	TargetNameText_1->SetAutoWrapText(true);
	TargetNameText_3->SetText(FText::FromString(CurrentBulletData->Effect)); // atk or def
	TargetNameText_3->SetAutoWrapText(true);

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

		if (TargetEnemy->IsA(AICC_Player::StaticClass()) && CurrentBulletData->Type != FearEV && CurrentBulletData->Type
			!= AngerEv &&
			CurrentBulletData->Type != JoyEv && CurrentBulletData->Type != CalmEV)
		{
			HideInfo();
			return;
		}

		if (TargetEnemy->IsA(AMob::StaticClass()) &&
			(CurrentBulletData->Type == AngerEv ||
				CurrentBulletData->Type == CalmEV ||
				CurrentBulletData->Type == JoyEv ||
				CurrentBulletData->Type == FearEV))
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
	if (!Displayer)
	{
		Displayer = CreateWidget<UBulletDisplayer>(GetWorld(), BulletDisplayerClass);
		BulletPanel->AddChild(Displayer);
	}

	AmmoSelectionIndicator->SetVisibility(GetBattleHandler()->IsControllerPlugged()
		                                      ? ESlateVisibility::Visible
		                                      : ESlateVisibility::Hidden);

	Displayer->Refresh();;
	SetSelectedBullet(0);
	GameInstance->GetCurrentPlayer()->GetBinder()->SetIsNavigatingInsideWidget(true);
	GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(Displayer);
	ApplyBulletDisplayerFocus();
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

void UBattleHUD::ApplyBulletDisplayerFocus()
{
	AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC)
	{
		DebugHelper::LogError("PC invalid on apply focus");
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(Displayer->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	PC->bShowMouseCursor = true;

	Displayer->SetUserFocus(PC);
	PC->GetLocalPlayer()->ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
}

void UBattleHUD::RevertBulletDisplayerFocus()
{
	AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC)
	{
		DebugHelper::LogError("PC invalid on RevertBulletDisplayerFocus");
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	InputMode.SetWidgetToFocus(TakeWidget());

	PC->SetInputMode(InputMode);

	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	PC->bShowMouseCursor = true;


	if (PC->GetLocalPlayer() && PC->GetLocalPlayer()->ViewportClient)
	{
		PC->GetLocalPlayer()->ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
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
	ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
	Displayer->SetVisibility(ESlateVisibility::Hidden);
	TargetText->SetVisibility(ESlateVisibility::Hidden);
	TargetNameText->SetVisibility(ESlateVisibility::Hidden);
	TargetText_2->SetVisibility(ESlateVisibility::Hidden);
	TargetText_3->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	Quantity->SetVisibility(ESlateVisibility::Hidden);
	Description->SetVisibility(ESlateVisibility::Hidden);
	bBulletSetupFinished = true;
	UBulletSelector::SetCanSelect(false);
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
		DebugHelper::LogError(
			"You can't add more ap than you have it current ap " + FString::FromInt(CurrentAP) + "- Accumulator " +
			FString::FromInt(ApAccumulator));
		DebugHelper::AddMessageToLog(
			"[BattleHUD]: You can't add more ap than you have it current ap " + FString::FromInt(CurrentAP) +
			"- Accumulator " + FString::FromInt(ApAccumulator));
		ApAccumulator = CurrentAP;
		Bar->UpdateHighlights(ApAccumulator);
	}


	switch (ApAccumulator)
	{
	case 1:
		{
			GameInstance->GetRuntimeStats().ApModifier = 1.0f;
			DebugHelper::AddMessageToLog(
				"[BattleHUD]: Player spent 1 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			DebugHelper::LogWarning(
				"[BattleHUD]: Player spent 1 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			break;
		}
	case 2:
		{
			GameInstance->GetRuntimeStats().ApModifier = 1.5f;
			DebugHelper::AddMessageToLog(
				"[BattleHUD]: Player spent 2 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			DebugHelper::LogWarning(
				"[BattleHUD]: Player spent 2 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			break;
		}
	case 3:
		{
			GameInstance->GetRuntimeStats().ApModifier = 2.0f;
			DebugHelper::AddMessageToLog(
				"[BattleHUD]: Player spent 3 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			DebugHelper::LogWarning(
				"[BattleHUD]: Player spent 3 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			break;
		}
	case 4:
		{
			GameInstance->GetRuntimeStats().ApModifier = 2.5f;
			DebugHelper::AddMessageToLog(
				"[BattleHUD]: Player spent 4 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			DebugHelper::LogWarning(
				"[BattleHUD]: Player spent 4 extra ap " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
			break;
		}

	default:
		{
			GameInstance->GetRuntimeStats().ApModifier = 1.0f;
			DebugHelper::AddMessageToLog(
				"[BattleHUD]: Player spent 1 extra ap (default) " + FString::SanitizeFloat(
					GameInstance->GetRuntimeStats().ApModifier));
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
	case 1: NewModifier = 1.0f;
		break;
	case 2: NewModifier = 1.5f;
		break;
	case 3: NewModifier = 2.0f;
		break;
	case 4: NewModifier = 2.5f;
		break;
	default: NewModifier = 1.0f;
		break;
	}

	GameInstance->GetRuntimeStats().ApModifier = NewModifier;
	DebugHelper::AddMessageToLog(
		"[BattleHUD]: Player decided to decrease boost, now has: " + FString::SanitizeFloat(NewModifier));

	Bar->UpdateHighlights(ApAccumulator);

	FSlateApplication::Get().ClearAllUserFocus();
}

void UBattleHUD::PrepareToEngage()
{
	bIsEvFirst = false;
	AMob* SelectedEnemy = Cast<AMob>(BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex]);
	checkf(SelectedEnemy, TEXT("SelectedEnemy is null at UBattleHUD::Engage"));
	SelectedActorTarget = SelectedEnemy;
	
	FDamage DummyDamage(CurrentBulletData, GameInstance->GetPlayerStats(),
	                    Cast<AMob>(SelectedActorTarget)->GetTactics(),
	                    Cast<AMob>(SelectedActorTarget)->GetData(), GameInstance->GetCurrentPlayer() ,GameInstance);

	GameInstance->GetRuntimeStats().Stats = DummyDamage;

	GameInstance->SetDamageData(&GameInstance->GetRuntimeStats().Stats);
	DebugHelper::LogMessage(3, FColor::White, "Targeting " + SelectedEnemy->GetActorLabel());
	DebugHelper::AddMessageToLog(
		"[BattleHUD]: Targeting " + SelectedEnemy->GetActorLabel() + " using " + CurrentBulletData->BulletName);
	checkf(MinigameHandler, TEXT("Minigame handler is null at UBattleHUD::Engage"));
	MinigameHandler->StartMinigame(CurrentBulletData,true);
	EngageBtn->SetVisibility(ESlateVisibility::Hidden);
	CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	Quantity->SetVisibility(ESlateVisibility::Hidden);
	ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Visible);
	ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Visible);
}

AMob* UBattleHUD::RetrieveSelectedTarget()
{
	AMob* SelectedEnemy = Cast<AMob>(BattleHandler->GetTurnBasedSystem()->GetTurn().Queue[CurrentEnemyIndex]);
	SelectedActorTarget = SelectedEnemy;
	return Cast<AMob>(SelectedActorTarget);
}

void UBattleHUD::PrepareToEngageEv(const EBuffStatus& BuffToReceive)
{
	bIsEvFirst = true;
	EngageBtn->SetVisibility(ESlateVisibility::Hidden);
	CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
	ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	SelectedActorTarget = GameInstance->GetCurrentPlayer();

	if (!SelectedActorTarget)
	{
		DebugHelper::LogError("Target is invalid - buff bullet");
		return;
	}

	GameInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(BuffToReceive);
	GetBulletDisplayer()->RemoveBullet();
	Displayer->Refresh();
	RefreshPistolMagazine();
	UpdateAp();
	EnableButtonsAfterShooting();
}

void UBattleHUD::PrepareToEngageEv(const EAfflictedStatus& StatusToInflict)
{
	bIsEvFirst = true;
	EngageBtn->SetVisibility(ESlateVisibility::Hidden);
	CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
	ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

	//SelectedActorTarget = RetrieveSelectedTarget();

	if (CurrentBulletData->Type == EBulletType::CalmEV)
	{
		SelectedActorTarget = GameInstance->GetCurrentPlayer();
	}
	else
	{
		SelectedActorTarget = RetrieveSelectedTarget();
	}

	if (!SelectedActorTarget)
	{
		DebugHelper::LogError("Target is invalid - status bullet");
		return;
	}

	GameInstance->GetCurrentPlayer()->GetStatusTracker()->InflictStatus(StatusToInflict, SelectedActorTarget);
	GetBulletDisplayer()->RemoveBullet();
	Displayer->Refresh();
	RefreshPistolMagazine();
	UpdateAp();
	EnableButtonsAfterShooting();
}

void UBattleHUD::PrepareToEngageEv(const EDebuffStatus& StatusToDebuff)
{
	bIsEvFirst = true;
	EngageBtn->SetVisibility(ESlateVisibility::Hidden);
	CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
	ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	SelectedActorTarget = RetrieveSelectedTarget();

	if (!SelectedActorTarget)
	{
		DebugHelper::LogError("Target is invalid - debuff bullet");
		return;
	}

	GameInstance->GetCurrentPlayer()->GetStatusTracker()->InflictDebuffStatus(StatusToDebuff, SelectedActorTarget);
	GetBulletDisplayer()->RemoveBullet();
	Displayer->Refresh();
	RefreshPistolMagazine();
	UpdateAp();
	EnableButtonsAfterShooting();
}

void UBattleHUD::MoveFocusOn(const float& Value)
{
	if (!DebugHelper::IsGamepadPlugged())
	{
		return;
	}
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	const int32 Direction = Value > 0 ? 1 : -1;
	TArray<UWidget*>& List = ActionPhaseButtons;
	CurrentWidgetIndex = (CurrentWidgetIndex + Direction + List.Num()) % List.Num();
	// GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(List[CurrentWidgetIndex]);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
	{
		GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(ActionPhaseButtons[CurrentWidgetIndex]);
	}, 0.01f, false);

	
	DebugHelper::LogMessage(9, FColor::White, "Scrolling on" + List[CurrentWidgetIndex]->GetName());
}

void UBattleHUD::SpawnVisualizer()
{
	FTimerHandle DelayHudHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHudHandle, [this]()
	{
		TSubclassOf<UVictoryVisualizer> VictoryVisualizerClass = LoadClass<UVictoryVisualizer>(
			this,TEXT("/Game/ICC/BluePrints/UI/Battle/BP_Victory.BP_Victory_C"));
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
	if (!Displayer)
	{
		return;
	}

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
	CanvasAmmoSelection->SetVisibility(ESlateVisibility::Hidden);
	ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
	ConfirmReloadBullet->SetVisibility(ESlateVisibility::Hidden);
	CanvasFirstReloadMagazine->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	Quantity->SetVisibility(ESlateVisibility::Hidden);
	Displayer->SetVisibility(ESlateVisibility::Hidden);
	Description->SetVisibility(ESlateVisibility::Hidden);
	SwitchToBattleUI();
	bBulletSetupFinished = true;
	bStartFight = true;
	GameInstance->GetCurrentPlayer()->GetBinder()->SetIsNavigatingInsideWidget(false);
	GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(Shoot);
	BattleHandler->GetTurnBasedSystem()->PrePrepareToBattle();
	RevertBulletDisplayerFocus();

	GameInstance->GetCurrentPlayer()->GetBinder()->SetIsNavigatingInsideWidget(false);
	CurrentWidgetIndex = 0;
	GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(ActionPhaseButtons[CurrentWidgetIndex]);
}

UAPBar* UBattleHUD::GetAPBar() const
{
	return Bar;
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
	if (!BattleHandler)
	{
		return;
	}
	
	if (bBulletSetupFinished && !GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn())
	{
		return;
	}
	
	if (!DebugHelper::IsGamepadPlugged())
	{
		DebugHelper::LogError("Controller is not plugged");
		return;
	}
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	CanvasBulletStats->SetVisibility(ESlateVisibility::Visible);
	BulletName->SetVisibility(ESlateVisibility::Visible);
	Quantity->SetVisibility(ESlateVisibility::Visible);
	TargetText->SetVisibility(ESlateVisibility::Visible);
	TargetText_2->SetVisibility(ESlateVisibility::Visible);

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
		CurrentSelectedBullet->SetCanBeSelected(true);
		CurrentSelectedBullet->DisplayBulletInfo();
		const FVector2D AbsPos = CurrentSelectedBullet->GetCachedGeometry().GetAbsolutePosition();
		const FVector2D LocalPos = AmmoSelectionIndicator->GetParent()->GetCachedGeometry().AbsoluteToLocal(AbsPos);

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

	CurrentBulletData = GetCircularBulletBuffer()->PeekAt(GetCircularBulletBuffer()->GetTailIndex());
	// this will take the first avaiable bullet
	checkf(CurrentBulletData, TEXT("Assigned CurrentBulletData invalid"))

	CanvasStatus->SetVisibility(ESlateVisibility::Hidden);
	BulletName->SetVisibility(ESlateVisibility::Hidden);
	HideInfo();

	switch (CurrentBulletData->Type)
	{
	case Indifference:
		{
			PrepareToEngage();
			break;
		}
	case AngerDv:
		{
			PrepareToEngage();
			break;
		}
	case AngerEv:
		{
			bIsEvFirst = true;
			PersistentInstance->GetCurrentPlayer()->GetStatusTracker()->BuffWith(EBuffStatus::AtkBuff);
			EngageBtn->SetVisibility(ESlateVisibility::Hidden);
			CanvasBulletStats->SetVisibility(ESlateVisibility::Hidden);
			GetBulletDisplayer()->RemoveBullet();
			RefreshPistolMagazine();
			Displayer->Refresh();
			UpdateAp();
			EnableButtonsAfterShooting();
			break;
		}
	case FearDv:
		{
			PrepareToEngage();
			break;
		}
	case FearEV:
		{
			PrepareToEngageEv(EBuffStatus::DefBuff);
			break;
		}
	case Disgust:
		{
			PrepareToEngage();
			break;
		}
	case DisgustEv:
		{
			PrepareToEngageEv(EDebuffStatus::DebuffAtk);
			break;
		}
	case SadnessDv:
		{
			PrepareToEngage();
			break;
		}
	case SadnessEv:
		PrepareToEngageEv(EDebuffStatus::DebuffDef);
		break;
	case JoyDv:
		{
			PrepareToEngage();
			break;
		}
	case JoyEv:
		{
			PrepareToEngageEv(EBuffStatus::LowHealth);
			break;
		}
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
			PrepareToEngageEv(EAfflictedStatus::ShieldDebuff);
			break;
		}
	case JealousyDv:
		{
			PrepareToEngage();
			break;
		}

	case JealousyEv:
		{
			PrepareToEngageEv(EAfflictedStatus::Burn);
			break;
		}
	case Shame:
		{
			PrepareToEngageEv(EAfflictedStatus::EAShame);
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

UBulletSelector* UBattleHUD::GetBulletSelector() const
{
	return CurrentSelectedBullet;
}

UBulletSelector* UBattleHUD::GetHoveredSelectedBullet() const
{
	return HoveredSelectedBullet;
}

void UBattleHUD::SetHoveredSelectedBullet(UBulletSelector* Hovered)
{
	HoveredSelectedBullet = Hovered;
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

	Player->GetRuntimeStats().CurrentHealth += CurrentBulletData->Power;
	const float Percentage = Player->GetRuntimeStats().CurrentHealth / Player->GetStats()->MaxHealth;
	PlayerHealth->SetPercent(Percentage);
	DebugHelper::LogWarning("Healed " + FString::SanitizeFloat(Percentage));
}

void UBattleHUD::ResetHealth()
{
	AICC_Player* Player = GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer();
	Player->GetRuntimeStats().CurrentHealth = Player->GetStats()->MaxHealth;
	const float Percentage = Player->GetRuntimeStats().CurrentHealth / Player->GetStats()->MaxHealth;
	PlayerHealth->SetPercent(Percentage);
}

void UBattleHUD::DisplayVictoryVisualizer()
{
	VictoryVisualizer->SetVisibility(ESlateVisibility::Visible);
	GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(VictoryVisualizer->GetButton());
}

void UBattleHUD::SpawnGameOverVisualizer()
{
	FTimerHandle DelayHudHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHudHandle, [this]()
	{
		TSubclassOf<UGameOverVisualizer> GameOverVisualizerClass = LoadClass<UGameOverVisualizer>(
			this,TEXT("/Game/ICC/BluePrints/UI/Battle/WBP_GameOver.WBP_GameOver_C"));
		checkf(GameOverVisualizerClass, TEXT("GameOverVisualizerClass path invalid"))
		GameOverVisualizer = CreateWidget<UGameOverVisualizer>(GetWorld(), GameOverVisualizerClass);
		checkf(GameOverVisualizer, TEXT("Invalid GameOverVisualizer"))
		VisualizerGameOverSlot->AddChild(GameOverVisualizer);
		GameOverVisualizer->SetVisibility(ESlateVisibility::Hidden);
		GameInstance->GetCurrentPlayer()->GetBinder()->FocusOn(GameOverVisualizer->GetButton());
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
	UBulletSelector* Bullet = nullptr;

	if (GetBattleHandler()->IsControllerPlugged())
		Bullet = CurrentSelectedBullet;
	else
		Bullet = HoveredSelectedBullet;

	if (Bullet)
	{
		CachedBulletQuantity = Bullet->GetBulletPtr()->GetQuantity();
	}

	return FText::FromString("Quantity: " + FString::FromInt(CachedBulletQuantity));
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

		if (AMob* Mob = Cast<AMob>(Queue[CheckIndex]); !Mob || Mob->IsAlive())
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

		if (AMob* Mob = Cast<AMob>(Queue[CheckIndex]); !Mob || Mob->IsAlive())
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

FBullet& UBattleHUD::GetCurrentSelectedBulletRef()
{
	return CurrentSelectedBullet->GetBullet();
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
	if (!Buffer) return;

	const int32 Tail = Buffer->GetTailIndex();
	const int32 Capacity = Buffer->GetCapacity();
	const int32 BulletCount = Buffer->GetCount();

	for (int32 i = 0; i < MagazineBullets.Num(); ++i)
	{
		if (!MagazineBullets[i]) continue;

		if (i < BulletCount)
		{
			const int32 BufferIndex = (Tail + i) % Capacity;
			UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

			MagazineBullets[i]->Setup(Buffer, BulletData, BufferIndex);
			MagazineBullets[i]->SetRenderOpacity(1.0f);
			MagazineBullets[i]->GetMagazineBulletButton()->SetBackgroundColor(BulletData->DisplayColor);
			MagazineBullets[i]->GetMagazineBulletButton()->SetIsEnabled(true);
		}
		else
		{
			MagazineBullets[i]->Setup(Buffer, nullptr, -1);
			MagazineBullets[i]->SetRenderOpacity(0.25f);
			MagazineBullets[i]->GetMagazineBulletButton()->SetBackgroundColor(FLinearColor::Transparent);
			MagazineBullets[i]->SetEnableRemoval(false);
		}
	}

	if (BulletCount == 0)
	{
		SelectedBulletIndex = 0;
		CurrentSelectedBullet = nullptr;
	}
}

void UBattleHUD::RefreshPistolMagazine()
{
	const UCircularBulletBuffer* Buffer = GetCircularBulletBuffer();
	if (!Buffer) return;

	const int32 Tail = Buffer->GetTailIndex();
	const int32 Cap = Buffer->GetCapacity();
	const int32 Count = Buffer->GetCount();

	for (int32 i = 0; i < PistolMagazines.Num(); ++i)
	{
		if (i < Count)
		{
			const int32 BufferIndex = (Tail + i) % Cap;
			if (const UBulletData* Data = Buffer->PeekAt(BufferIndex); Data)
			{
				PistolMagazines[i]->SetBrushFromTexture(Data->Icon, true);
				PistolMagazines[i]->SetColorAndOpacity(FLinearColor::White);
			}
			else
			{
				PistolMagazines[i]->SetBrushFromTexture(nullptr);
				PistolMagazines[i]->SetColorAndOpacity(FLinearColor::Transparent);
			}
		}

		else
		{
			PistolMagazines[i]->SetBrushFromTexture(nullptr);
			PistolMagazines[i]->SetColorAndOpacity(FLinearColor::White);
		}
	}
}

void UBattleHUD::ResetPistolMagazine()
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
		if (!BulletSlot) continue;
		BulletSlot->Setup(nullptr, nullptr, -1);
		BulletSlot->SetEnableRemoval(true);
	}

	for (UImage* Img : PistolMagazines)
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

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBattleHUD::ApplyBulletDisplayerFocus);
}

int32 UBattleHUD::GetSelectedBulletIndex() const
{
	return SelectedBulletIndex;
}

void UBattleHUD::SetSelectedBulletIndex(const int32& Value)
{
	SelectedBulletIndex = Value;
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
