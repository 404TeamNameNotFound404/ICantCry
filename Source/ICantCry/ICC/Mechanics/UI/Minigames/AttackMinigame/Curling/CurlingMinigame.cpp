// Fill out your copyright notice in the Description page of Project Settings.


#include "CurlingMinigame.h"

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
}

void UCurlingMinigame::MoveSlider(const FVector2D& Position)
{
	Super::MoveSlider(Position);
}

void UCurlingMinigame::HandleScore()
{
	Super::HandleScore();
}

EMinigameThreshold UCurlingMinigame::CheckBar()
{
	return Super::CheckBar();
}
