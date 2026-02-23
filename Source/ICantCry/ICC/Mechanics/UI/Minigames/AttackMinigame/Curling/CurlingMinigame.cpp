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
	
	if (ScrollValue > 0)
	{
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
		Slider->SetPercent(Percentage);
		const FVector2D InitialBallPosition = Ball->GetCachedGeometry().GetAbsolutePosition();
			
		switch (MinigameResult)
		{
		default:
		case Miss:
		case Bad:
			{
				BallTargetEndingPoint= BlueBallPos->GetCachedGeometry().GetAbsolutePosition(); 
				BallEndingPosition = FMath::Lerp(InitialBallPosition, BallTargetEndingPoint, GetWorld()->GetDeltaSeconds() * BallSpeed);
				Ball->SetRenderTranslation(BallEndingPosition);
			}
			break;
		case Good:
			break;
		case Perfect:
			break;
		}
	}
	
	// Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
	// Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
	// Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
	// HandleScore();
	// Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
}

void UCurlingMinigame::HandleScore()
{
	switch (MinigameResult)
	{
	default:
	case Miss:
	case Bad:
		{
			Instance->GetRuntimeStats().MinigameModifier = 0.5f;
			Instance->GetRuntimeStats().MinigameModifier = 0.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
			{
				Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
					EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
			}
			
			DebugHelper::LogMessage(9, FColor::Black, "Bad anger minigame score");
			break;
		}
	case Good:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		
			if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
			{
				Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
					EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
			}
			
			DebugHelper::LogMessage(9, FColor::Black, "Good anger minigame score");
			break;
		}
		
	case Perfect:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		
			if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
			{
				Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
					EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
			}
		
			DebugHelper::LogMessage(9, FColor::Black, "Perfect anger minigame score");
			break;
		}
	}
}

EMinigameThreshold UCurlingMinigame::CheckBar()
{
	const float CurrentPercent = Slider->GetPercent();
	
	const float Dist = FMath::Abs(CurrentPercent - 0.5f);

	if (Dist <= PerfectScoreRange)
	{
		DebugHelper::LogSuccess("Perfect Score");
		return EMinigameThreshold::Perfect;
	}
	
	if (Dist <= GoodScoreRange)
	{
		DebugHelper::LogWarning("Not bad");
		return EMinigameThreshold::Good;
	}
	
	if (Dist <= BadScoreRange)
	{
		DebugHelper::LogError("Shit game");
		return EMinigameThreshold::Bad;
	}

	return EMinigameThreshold::Miss;
}

void UCurlingMinigame::Flow()
{
	
}

float UCurlingMinigame::GetPercentFromImage(UImage* Image)
{
	if (!Image || !Slider) return 0.0f;

	const FGeometry& SliderGeo = Slider->GetCachedGeometry();
	const FVector2D RelativePos = SliderGeo.AbsoluteToLocal(Image->GetCachedGeometry().GetAbsolutePosition());
	const float NormalizedPercent = RelativePos.X / SliderGeo.GetLocalSize().X;

	return FMath::Clamp(NormalizedPercent, 0.0f, 1.0f);
}
