// Fill out your copyright notice in the Description page of Project Settings.
#include "MinigameUserWidget.h"

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

void UMinigameUserWidget::MoveSlider(const FVector2D& Position)
{
	
}
