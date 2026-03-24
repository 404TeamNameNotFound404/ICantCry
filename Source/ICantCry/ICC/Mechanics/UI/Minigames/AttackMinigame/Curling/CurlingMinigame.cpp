// Fill out your copyright notice in the Description page of Project Settings.
#include "CurlingMinigame.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UCurlingMinigame::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (DebugHelper::IsGamepadPlugged())
	{
		KeyIcon->SetBrushFromTexture(IconMap["Pad_X"]);
	}
	else
	{
		KeyIcon->SetBrushFromTexture(IconMap["Key_SpaceBar"]);
	}
	
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	Slider->SetPercent(0.0f);
	Slider->SetFillColorAndOpacity(FLinearColor::Blue);
	
	BallStartingPosition = Ball->GetCachedGeometry().GetAbsolutePosition();
}

void UCurlingMinigame::MoveSlider(const FVector2D& Position)
{
	float Percentage = Slider->GetPercent();
	
	if (const bool InputPressed = Instance->GetCurrentPlayer()->GetBinder()->GetIsCurlingPressed();
		InputPressed)
	{
		bPressed = true;
		
		if ( !Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
		{
			Percentage += ScrollValue * GetWorld()->GetDeltaSeconds() * SliderSpeed;
			Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
			Slider->SetPercent(Percentage);
			MinigameResult = CheckBar();
		}
	}
	else
	{
		if (bPressed)
		{
			Process();
		}
	}
}

void UCurlingMinigame::HandleScore()
{
	switch (MinigameResult)
	{
	default:
	case Miss:
		{
			Instance->GetRuntimeStats().MinigameModifier = 0.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			DebugHelper::LogMessage(9, FColor::Black, "Miss curling minigame score");
		}
		break;
	case Bad:
		{
			Instance->GetRuntimeStats().MinigameModifier = 0.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			
			DebugHelper::LogMessage(9, FColor::Black, "Not Bad curling minigame score");
			break;
		}
	case Good:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			
			DebugHelper::LogMessage(9, FColor::Black, "Good curling minigame score");
			break;
		}
		
	case Perfect:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			DebugHelper::LogMessage(9, FColor::Black, "Perfect curling minigame score");
			break;
		}
	}
}

EMinigameThreshold UCurlingMinigame::CheckBar()
{
	const float P = Slider->GetPercent();
	
	if (P >= PerfectScoreRange && P <= PerfectScoreRangeMax) 
	{
		return EMinigameThreshold::Perfect;
	}
	
	else if (P >= GoodScoreRange && P <= GoodScoreRangeMax)
	{
		return EMinigameThreshold::Good;
	}
	
	else if (P >= BadScoreRange && P <= BadScoreRangeMax)
	{
		return EMinigameThreshold::Bad;
	}
	
	DebugHelper::LogMessage(6, FColor::White, "Bar y pos " + FString::SanitizeFloat(Slider->GetPercent()));

	return EMinigameThreshold::Miss;
	
}

void UCurlingMinigame::Flow()
{
	
}

void UCurlingMinigame::Process()
{
	UWidget* TargetWidget = nullptr;

	switch (MinigameResult)
	{
	case Good:
		TargetWidget = WhiteBallPos;
		break;

	case Perfect:
		TargetWidget = RedBallPos;
		break;

	case Miss:
	case Bad:
	default:
		TargetWidget = BlueBallPos;
		break;
	}

	if (!TargetWidget) return;

	const FGeometry& BallGeometry   = Ball->GetCachedGeometry();
	const FGeometry& TargetGeometry = TargetWidget->GetCachedGeometry();
	const FVector2D BallAbsolute   = BallGeometry.GetAbsolutePosition();
	const FVector2D TargetAbsolute = TargetGeometry.GetAbsolutePosition();
	
	const FVector2D DeltaAbsolute = TargetAbsolute - BallAbsolute;
	
	const FVector2D CurrentTranslation = Ball->GetRenderTransform().Translation;
	const FVector2D TargetTranslation  = CurrentTranslation + DeltaAbsolute;

	const FVector2D NewTranslation = FMath::Vector2DInterpTo(
		CurrentTranslation,
		TargetTranslation,
		GetWorld()->GetDeltaSeconds(),
		BallSpeed
	);
	
	if (FVector2D::Distance(NewTranslation, TargetTranslation) < 1.0f)
	{
		Ball->SetRenderTranslation(TargetTranslation);
		
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		HandleScore();
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
	}
	else
	{
		Ball->SetRenderTranslation(NewTranslation);
	}
}

float UCurlingMinigame::GetPercentFromImage(UImage* Image)
{
	if (!Image || !Slider) return 0.0f;

	const FGeometry& SliderGeo = Slider->GetCachedGeometry();
	const FVector2D RelativePos = SliderGeo.AbsoluteToLocal(Image->GetCachedGeometry().GetAbsolutePosition());
	const float NormalizedPercent = RelativePos.X / SliderGeo.GetLocalSize().X;

	return FMath::Clamp(NormalizedPercent, 0.0f, 1.0f);
}

FString UCurlingMinigame::GetThresholdName(const EMinigameThreshold& Threshold) const
{
	switch (Threshold)
	{
	case Bad:
		return "Bad";
	case Good:
		return "Good";
	case Perfect:
		return "Perfect";
	default:
	case Miss:
		return "MISS";
	}
}
