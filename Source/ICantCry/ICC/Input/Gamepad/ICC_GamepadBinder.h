// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/Widget.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/AttackMinigame/NoteHighway/NoteHighwayMinigame.h"
#include "UObject/Object.h"
#include "ICC_GamepadBinder.generated.h"

class AICC_Player;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UICC_GamepadBinder : public UActorComponent
{
	GENERATED_BODY()

public:

	void Init(AICC_Player* inPlayer);
	
	void Input_GamepadSelectionInteraction(const FInputActionValue& InputActionValue);
	void Input_GamepadEngageInteraction(const FInputActionValue& InputActionValue);
	void Input_GamepadRemoveBullet(const FInputActionValue& InputActionValue);
	void Input_GamepadShootBoost(const FInputActionValue& InputActionValue);
	void Input_GamepadNavigateUi(const FInputActionValue& InputActionValue);
	void Input_GamepadSimulateClick(const FInputActionValue& InputActionValue);
	void Input_GamepadMinigameRelease(const FInputActionValue& InputActionValue);
	void Input_GamepadMinigameGuitarHero_X(const FInputActionValue& InputActionValue);
	void Input_GamepadMinigameGuitarHero_Y(const FInputActionValue& InputActionValue);
	void Input_GamepadMinigameGuitarHero_A(const FInputActionValue& InputActionValue);
	void Input_GamepadMinigameGuitarHero_B(const FInputActionValue& InputActionValue);

	void FocusOn(UWidget* Target);
	UWidget* GetCurrentFocusedWidget() const;

	bool IsNavigating() const;
	void SetIsNavigatingInsideWidget(const bool& inIsNavigating);
	
	bool GetDecreaseMinigameScrollValue() const;
	void SetDecreaseMinigameScrollValue(const bool& Value);

private:
	UPROPERTY()
	AICC_Player* Player = nullptr;

	UPROPERTY()
	UWidget* CurrentFocusedWidget = nullptr;

	UPROPERTY()
	bool bNavigateInWidget = false;

	UPROPERTY()
	float LastNavigationAxis = 0.0f;

	UPROPERTY()
	bool bWasSimulationButtonPressed = false;
	
	UPROPERTY()
	bool bDecreaseScrollValueMinigame = false;
	
	void SimulateClick();
	
	void ProcessGuitarHeroInput(const ESpawnableHighwayBtn& RowType);
};
