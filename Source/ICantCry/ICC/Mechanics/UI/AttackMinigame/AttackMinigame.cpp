// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackMinigame.h"

#include "Components/PanelWidget.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UAttackMinigame::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAttackMinigame::MoveSlider(const FVector2D& Position)
{
	checkf(Slider, TEXT("Slider in UDefenceMinigame::MoveSlider is null"));
	StartingSliderPosition = Slider->GetRenderTransform().Translation;
	
	if (bStopSlider)
	{
		DebugHelper::LogMessage(5, FColor::Black, "Bar Stopped at " + Slider->GetRenderTransform().Translation.ToString());
		return;
	}
	
	FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;
	CurrentPosition.Y = 0;
	FVector2D DeltaMove = Position * Speed * GetWorld()->GetDeltaSeconds();
	FVector2D NewPosition = CurrentPosition + DeltaMove;
	NewPosition.Y = 0;
	Slider->SetRenderTranslation(NewPosition);
}

EMinigameThreshold UAttackMinigame::CheckBar()
{
	DebugHelper::LogWarning("Slider Position " + Slider->GetRenderTransform().Translation.ToString());
	const FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;

	if (CurrentPosition.X >= 496.0f || CurrentPosition.X >= 500.0f)
	{
		return EMinigameThreshold::Perfect;
	}

	if ((CurrentPosition.X >= 400 && CurrentPosition.X < 496.0f))
	{
		return EMinigameThreshold::Good;
	}

	if (CurrentPosition.X > 503.0f)
	{
		return EMinigameThreshold::Good; 
	}

	if (CurrentPosition.X >= 10.0f && CurrentPosition.X <= 400.0f)
	{
		return EMinigameThreshold::Bad;
	}
	
	if (CurrentPosition.X > 580.0f)
	{
		return EMinigameThreshold::Bad;
	}
	
	return EMinigameThreshold::Miss;
}

void UAttackMinigame::HandleScore()
{
	const EMinigameThreshold Result = CheckBar();

	switch (Result)
	{
		case EMinigameThreshold::Bad:
			DebugHelper::LogError("Bad minigame score!");
			break;
		case EMinigameThreshold::Good:
			DebugHelper::LogWarning("Good minigame score!");
			break;
		case EMinigameThreshold::Perfect:
			DebugHelper::LogSuccess("Perfect minigame score!");
			break;
		default:
			DebugHelper::LogMessage(3, FColor::FromHex("ADB2D4"),"Unknown minigame score!");
			break;
	}
}
