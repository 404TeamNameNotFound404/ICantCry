// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackMinigame.h"

#include "Components/PanelWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UAttackMinigame::NativeConstruct()
{
	Super::NativeConstruct();
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
}

EMinigameThreshold UAttackMinigame::CheckBar()
{
	const FVector2D CurrentPosition = Slider->GetCachedGeometry().GetAbsolutePosition();

	const float X = CurrentPosition.X;

	const float LeftDangerX = DangerBorderLeft->GetCachedGeometry().GetAbsolutePosition().X;
	const float LeftSafeX = SafeAreaLeft->GetCachedGeometry().GetAbsolutePosition().X;
	const float PerfectLeftX = PerfectAreaLeft->GetCachedGeometry().GetAbsolutePosition().X;
	const float PerfectRightX = PerfectAreaRight->GetCachedGeometry().GetAbsolutePosition().X;
	const float RightSafeX = SafeAreaRight->GetCachedGeometry().GetAbsolutePosition().X;
	const float RightDangerX = DangerBorderRight->GetCachedGeometry().GetAbsolutePosition().X;


	if (X >= PerfectLeftX && X <= PerfectRightX)
	{
		return EMinigameThreshold::Perfect;
	}

	if ((X >= LeftSafeX && X < PerfectLeftX) || (X > PerfectRightX && X <= RightSafeX))
	{
		return EMinigameThreshold::Good;
	}

	if ((X >= LeftDangerX && X < LeftSafeX) || (X > RightSafeX && X <= RightDangerX))
	{
		return EMinigameThreshold::Bad;
	}

	return EMinigameThreshold::Miss;
}

// ----------------REMINDER-------------------------------------
// calculated at runtime depending on the type of emotions target
void UAttackMinigame::HandleScore()
{
	const EMinigameThreshold Result = CheckBar();
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	switch (Result)
	{
	case EMinigameThreshold::Bad:
		DebugHelper::LogError("Bad minigame score!");
		Instance->GetPlayerStats()->MinigameModifier = 0.5f;
		DebugHelper::LogWarning("Minigame modifier On Bad" + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));
		DebugHelper::AddMessageToLog("Player Minigame modifier " + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier) + "\nBad minigame score!");
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);

		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}

		
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		break;
	case EMinigameThreshold::Good:
		DebugHelper::LogWarning("Good minigame score!");
		Instance->GetPlayerStats()->MinigameModifier = 1.0f;
		DebugHelper::LogWarning("Minigame modifier on Good " + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));
		DebugHelper::AddMessageToLog("Player Minigame modifier " + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));
		DebugHelper::AddMessageToLog("Good minigame score!");
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			// Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion()->GetStatusTracker()->InflictStatus(
			// 	EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion());
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		break;
	case EMinigameThreshold::Perfect:
		DebugHelper::LogSuccess("Perfect minigame score!");
		Instance->GetPlayerStats()->MinigameModifier = 1.5f;
		DebugHelper::LogWarning("Minigame modifier on Perfect" + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));
		DebugHelper::AddMessageToLog("Player Minigame modifier " + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));
		DebugHelper::AddMessageToLog("Perfect minigame score!");
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			// Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion()->GetStatusTracker()->InflictStatus(
			// 	EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion());
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		break;
	default:
		DebugHelper::LogMessage(3, FColor::FromHex("ADB2D4"), "Unknown minigame score!");
		Instance->GetPlayerStats()->MinigameModifier = 0.5f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			// Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion()->GetStatusTracker()->InflictStatus(
			// 	EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedEmotion());
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		break;
	}
}
