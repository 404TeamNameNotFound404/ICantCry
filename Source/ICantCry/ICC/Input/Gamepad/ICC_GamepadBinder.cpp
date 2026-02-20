// Fill out your copyright notice in the Description page of Project Settings.
#include "ICC_GamepadBinder.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UICC_GamepadBinder::Init(AICC_Player* inPlayer)
{
	Player = inPlayer;
	DebugHelper::LogMessage(8, FColor::White, "PadBinder init complete");
}

void UICC_GamepadBinder::Input_GamepadSelectionInteraction(const FInputActionValue& InputActionValue)
{
	if (!DebugHelper::IsGamepadPlugged() || !Player->bIsInFight || Player->GetBattleHUD()->IsBulletSelectionOver())
	{
		return;
	}

	if (!Player->GetBattleHUD()->GetBulletDisplayer() ||
		!Player->GetBattleHUD()->GetBulletDisplayer()->GetBullets().IsValidIndex(
			Player->GetBattleHUD()->GetSelectedBulletIndex()))
	{
		return;
	}

	const auto SelectedBullet = Player->GetBattleHUD()->GetBulletDisplayer()->GetBullets()[Player->GetBattleHUD()->
		GetSelectedBulletIndex()];

	if (!SelectedBullet)
	{
		return;
	}

	if (Player->GetBattleHUD()->IsSelectingTarget())
	{
		return;
	}

	SelectedBullet->AddToRevolver();
}

void UICC_GamepadBinder::Input_GamepadEngageInteraction(const FInputActionValue& InputActionValue)
{
	if (!DebugHelper::IsGamepadPlugged() || !Player->bIsInFight || !Player->GetBattleHUD()->IsBulletSelectionOver())
	{
		DebugHelper::LogError(
			"Broooooo Bullet selection is not over or bisngight is false or controller is not plugged in correctly");
		return;
	}

	if (!Player->GetBattleHUD()->IsSelectingTarget())
	{
		DebugHelper::LogError("Broooooo Is selecting target is false!");
		return;
	}

	Player->GetBattleHUD()->Engage();
}

void UICC_GamepadBinder::Input_GamepadRemoveBullet(const FInputActionValue& InputActionValue)
{
	if (!DebugHelper::IsGamepadPlugged() || !Player->GetBattleHUD()->GetBattleHandler())
	{
		return;
	}

	if (!Player->bIsInFight)
	{
		return;
	}

	if (Player->GetBattleHUD()->IsBulletSelectionOver())
	{
		return;
	}

	Player->GetBattleHUD()->MagazineBullets[0]->Clear();
}


void UICC_GamepadBinder::Input_GamepadShootBoost(const FInputActionValue& InputActionValue)
{
	if (!DebugHelper::IsGamepadPlugged() || !Player->GetBattleHUD()->GetBattleHandler())
	{
		return;
	}

	if (!Player->bIsInFight)
	{
		return;
	}

	if (!Player->GetBattleHUD()->IsBulletSelectionOver())
	{
		return;
	}

	if (!Player->GetBattleHUD()->IsSelectingTarget() && !Player->GetBattleHUD()->GetBattleHandler()->
	                                                             GetTurnBasedSystem()->GetIsPlayerTurn())
	{
		return;
	}

	const float Value = InputActionValue.Get<float>();
	
	if (constexpr float DeadZone = 0.15f;
		FMath::Abs(Value) < DeadZone)
	{
		return;
	}

	if (Value > 0.0f)
	{
		Player->GetBattleHUD()->IncreaseShootPower();
	}
	else
	{
		Player->GetBattleHUD()->DecreaseShootPower();
	}
}

void UICC_GamepadBinder::Input_GamepadNavigateUi(const FInputActionValue& InputActionValue)
{
	if (!Player || !Player->GetBattleHUD()) return;
	if (!Player->bIsInFight) return;

	UBattleHUD* HUD = Player->GetBattleHUD();
	if (!HUD->GetBattleHandler() || !HUD->GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn()) return;

	const float Axis = InputActionValue.Get<float>();
	
	if (constexpr float DeadZone = 0.15f;
		FMath::Abs(Axis) < DeadZone) // FMath::IsNearlyZero(Axis)
	{
		LastNavigationAxis = 0.0f;
		return;
	}

	if ((Axis > 0.0f && LastNavigationAxis <= 0.0f) ||
		(Axis < 0.0f && LastNavigationAxis >= 0.0f))
	{
		if (bNavigateInWidget)
		{
			HUD->ScrollBulletSelection(FMath::Sign(Axis));
		}

		else if (HUD->IsBulletSelectionOver())
		{
			HUD->MoveFocusOn(FMath::Sign(Axis));
		}
	}

	LastNavigationAxis = Axis;
}

void UICC_GamepadBinder::Input_GamepadSimulateClick(const FInputActionValue& InputActionValue)
{
	if (!bWasSimulationButtonPressed && InputActionValue.Get<bool>())
	{
		SimulateClick();
		bWasSimulationButtonPressed = true;
	}
	else if (!InputActionValue.Get<bool>())
	{
		bWasSimulationButtonPressed = false;
	}
}

void UICC_GamepadBinder::Input_GamepadMinigameRelease(const FInputActionValue& InputActionValue)
{
	if (!Player->GetCurrentMinigameDisplayed())
	{
		return;
	}

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [&]
	{
		bDecreaseScrollValueMinigame = true;
		DebugHelper::LogMessage(15, FColor::White, "Minigame released!");
	}, 0.25f, false);
}

void UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_X(const FInputActionValue& InputActionValue)
{
	ProcessGuitarHeroInput(ESpawnableHighwayBtn::X);
}

void UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_Y(const FInputActionValue& InputActionValue)
{
	ProcessGuitarHeroInput(ESpawnableHighwayBtn::Y);
}

void UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_A(const FInputActionValue& InputActionValue)
{
	ProcessGuitarHeroInput(ESpawnableHighwayBtn::A);
}

void UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_B(const FInputActionValue& InputActionValue)
{
	ProcessGuitarHeroInput(ESpawnableHighwayBtn::B);
}


void UICC_GamepadBinder::FocusOn(UWidget* Target)
{
	if (!DebugHelper::IsGamepadPlugged() || !Target)
	{
		return;
	}

	AICC_PlayerController* PC = nullptr;
	
	if (const APawn* Owner = Cast<APawn>(GetOwner()))
	{
		PC = Cast<AICC_PlayerController>(Owner->GetController());
	}
	
	if (!PC)
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(Target->TakeWidget());

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->bShowMouseCursor = false;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	PC->SetInputMode(InputMode);

	Target->SetUserFocus(PC);
	CurrentFocusedWidget = Target;
}

UWidget* UICC_GamepadBinder::GetCurrentFocusedWidget() const
{
	return CurrentFocusedWidget;
}

void UICC_GamepadBinder::SimulateClick()
{
	if (!DebugHelper::IsGamepadPlugged() || !CurrentFocusedWidget)
	{
		return;
	}

	if (UButton* Btn = Cast<UButton>(CurrentFocusedWidget))
	{
		Btn->OnClicked.Broadcast();
	}
}

void UICC_GamepadBinder::ProcessGuitarHeroInput(const ESpawnableHighwayBtn& RowType)
{
	if (!Player->bIsInFight)
	{
		return;
	}
	
	if (Player->GetBattleHUD()->GetCurrentBulletData()->MinigameTemplate != EMinigameType::GuitarHero
		&& !Player->GetIsMinigameInputEnabled())
	{
		return;
	}
	
	Cast<UNoteHighwayMinigame>(Player->GetCurrentMinigameDisplayed())->Simulate(RowType);
}

bool UICC_GamepadBinder::IsNavigating() const
{
	return bNavigateInWidget;
}

void UICC_GamepadBinder::SetIsNavigatingInsideWidget(const bool& inIsNavigating)
{
	bNavigateInWidget = inIsNavigating;
}

bool UICC_GamepadBinder::GetDecreaseMinigameScrollValue() const
{
	return bDecreaseScrollValueMinigame;
}

void UICC_GamepadBinder::SetDecreaseMinigameScrollValue(const bool& Value)
{
	bDecreaseScrollValueMinigame = Value;
}
