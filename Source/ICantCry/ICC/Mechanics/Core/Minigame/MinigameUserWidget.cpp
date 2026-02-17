// Fill out your copyright notice in the Description page of Project Settings.
#include "MinigameUserWidget.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UMinigameUserWidget::SetStopSlider(const bool& Stop)
{
	bStopSlider = Stop;
}

EMinigameThreshold UMinigameUserWidget::CheckBar()
{
	return EMinigameThreshold::Miss;
}

void UMinigameUserWidget::HandleScore()
{
}

void UMinigameUserWidget::Flow()
{
	CheckBar();
	HandleScore();
}

bool UMinigameUserWidget::OffsideChecker()
{
	return false;
}

void UMinigameUserWidget::Handle(UBulletData* BulletData, AMinigameHandler* MinigameHandler)
{
	if (!BulletData)
	{
		DebugHelper::LogError("Bullet data is null!");
		MinigameHandler->EndMinigame();
		return;
	}
	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	switch (BulletData->MinigameTemplate)
	{
	default:
	case Default:
		SetStopSlider(true);
		MinigameHandler->EndMinigame();
		break;
	case Anger:
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		break;
	case GuitarHero:
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		break;
	case Curling:
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		break;
	}
}

void UMinigameUserWidget::SetScrollValue(const float& Value)
{
	ScrollValue = Value;
}

float& UMinigameUserWidget::GetScrollValue()
{
	return ScrollValue;
}

void UMinigameUserWidget::MoveSlider(const FVector2D& Position)
{
	
}
