// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackMinigame.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UAttackMinigame::NativeConstruct()
{
	Super::NativeConstruct();
}

static FString GetThresholdName(const EMinigameThreshold& T)
{
	switch (T)
	{
	case EMinigameThreshold::Perfect:
		return "PERFECT";
	case EMinigameThreshold::Good:
		return "GOOD";
	default:
	case EMinigameThreshold::Bad:
		return "BAD";
	}
}

void UAttackMinigame::MoveSlider(const FVector2D& Position)
{
	checkf(Slider, TEXT("Slider in UDefenceMinigame::MoveSlider is null"));
	StartingSliderPosition = Slider->GetRenderTransform().Translation;

	FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;
	CurrentPosition.Y = 0;
	FVector2D DeltaMove = Position * Speed * GetWorld()->GetDeltaSeconds() * MovementDirection;
	FVector2D NewPosition = CurrentPosition + DeltaMove;

	if (NewPosition.X > BorderRight)
	{
		NewPosition.X = BorderRight;
		MovementDirection *= -1;
	}
	else if (NewPosition.X < BorderLeft)
	{
		NewPosition.X = BorderLeft;
		MovementDirection *= -1;
	}

	NewPosition.Y = 0;
	Slider->SetRenderTranslation(NewPosition);

	T = CheckBar();
	DebugHelper::LogSuccess("Hey look " + GetThresholdName(T));
}

EMinigameThreshold UAttackMinigame::CheckBar()
{
	checkf(Slider, TEXT("Slider is null"));

	const FGeometry& SliderGeometry = Slider->GetCachedGeometry();
	const FVector2D SliderAbsolutePos = USlateBlueprintLibrary::GetAbsoluteSize(SliderGeometry) * 0.5f + SliderGeometry.
		GetAbsolutePosition();
	const FGeometry& MyGeometry = GetCachedGeometry();

	float SliderLocalX = USlateBlueprintLibrary::AbsoluteToLocal(MyGeometry, SliderGeometry.GetAbsolutePosition()).X;
	float LeftDangerX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, DangerBorderLeft->GetCachedGeometry().GetAbsolutePosition()).X;
	float LeftSafeX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, SafeAreaLeft->GetCachedGeometry().GetAbsolutePosition()).X;
	float PerfectLeftX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, PerfectAreaLeft->GetCachedGeometry().GetAbsolutePosition()).X;
	float PerfectRightX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, PerfectAreaRight->GetCachedGeometry().GetAbsolutePosition()).X;
	float RightSafeX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, SafeAreaRight->GetCachedGeometry().GetAbsolutePosition()).X;
	float RightDangerX = USlateBlueprintLibrary::AbsoluteToLocal(
		MyGeometry, DangerBorderRight->GetCachedGeometry().GetAbsolutePosition()).X;

	DebugHelper::LogMessage(9, FColor::White,
	                        "LOCAL - Slider X: " + FString::SanitizeFloat(SliderLocalX) + " Perfect Left: " +
	                        FString::SanitizeFloat(PerfectLeftX));


	if (SliderLocalX >= PerfectLeftX && SliderLocalX <= PerfectRightX)
	{
		DebugHelper::LogSuccess("Oh no perfect");
		return EMinigameThreshold::Perfect;
	}

	if ((SliderLocalX >= LeftSafeX && SliderLocalX < PerfectLeftX) || (SliderLocalX > PerfectRightX && SliderLocalX <=
		RightSafeX))
	{
		DebugHelper::LogSuccess("Oh no good");
		return EMinigameThreshold::Good;
	}

	if ((SliderLocalX >= LeftDangerX && SliderLocalX < LeftSafeX) || (SliderLocalX > RightSafeX && SliderLocalX <=
		RightDangerX))
	{
		DebugHelper::LogSuccess("Oh no ERROR");
		return EMinigameThreshold::Bad;
	}

	return EMinigameThreshold::Miss;
}

// ----------------REMINDER-------------------------------------
// calculated at runtime depending on the type of emotions target
void UAttackMinigame::HandleScore()
{
	// const EMinigameThreshold Result = CheckBar();
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	switch (T)
	{
	default:
	case EMinigameThreshold::Bad:
		DebugHelper::LogError("Bad minigame score!");
		Instance->GetRuntimeStats().MinigameModifier = 0.5f;
		DebugHelper::LogWarning(
			"Minigame modifier On Bad" + FString::SanitizeFloat(Instance->GetRuntimeStats().MinigameModifier));
		DebugHelper::AddMessageToLog(
			"[Attack Minigame]: Player Minigame modifier " + FString::SanitizeFloat(
				Instance->GetRuntimeStats().MinigameModifier) + " - Bad minigame score!");
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetPlayerStats()->RuntimeStats.ApModifier = 0;
		break;
	case EMinigameThreshold::Good:
		DebugHelper::LogWarning("Good minigame score!");
		Instance->GetRuntimeStats().MinigameModifier = 1.0f;
		DebugHelper::LogWarning(
			"Minigame modifier on Good " + FString::SanitizeFloat(Instance->GetRuntimeStats().MinigameModifier));
		DebugHelper::AddMessageToLog(
			"[Attack Minigame]: Player Minigame modifier " + FString::SanitizeFloat(
				Instance->GetRuntimeStats().MinigameModifier));
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();

		Instance->GetPlayerStats()->RuntimeStats.ApModifier = 0;
		break;
	case EMinigameThreshold::Perfect:
		DebugHelper::LogSuccess("Perfect minigame score!");
		Instance->GetRuntimeStats().MinigameModifier = 1.5f;
		DebugHelper::LogWarning(
			"Minigame modifier on Perfect" + FString::SanitizeFloat(Instance->GetRuntimeStats().MinigameModifier));
		DebugHelper::AddMessageToLog(
			"[Attack Minigame]: Player Minigame modifier " + FString::SanitizeFloat(
				Instance->GetRuntimeStats().MinigameModifier));
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetPlayerStats()->RuntimeStats.ApModifier = 0;
		break;
	}
}
