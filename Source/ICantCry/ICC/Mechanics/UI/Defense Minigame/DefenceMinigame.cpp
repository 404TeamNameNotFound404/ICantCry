// Fill out your copyright notice in the Description page of Project Settings.
#include "DefenceMinigame.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UDefenceMinigame::NativeConstruct()
{
	Super::NativeConstruct();
}

EMinigameThreshold UDefenceMinigame::CheckBar()
{
	const float DistanceToFirst = FVector2D::Distance(Slider->GetRenderTransform().Translation, BadDefence);
	const float DistanceToSecond = FVector2D::Distance(Slider->GetRenderTransform().Translation, GoodDefence);
	const float DistanceToParry = FVector2D::Distance(Slider->GetRenderTransform().Translation, PerfectParry);

	DebugHelper::LogMessage(3, FColor::Cyan , "Hello Checkbar");

	if (DistanceToFirst <= BadThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("88304E"), "Bad threshold hit");
		return EMinigameThreshold::Bad;
	}
		
	else if (DistanceToSecond <= GoodThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("4F1C51"), "Bad threshold hit");
		return EMinigameThreshold::Good;
	}
		
	else if (DistanceToParry <= PerfectThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("FFAB5B"), "Bad threshold hit");
		return EMinigameThreshold::Perfect;
	}
	
	return EMinigameThreshold::Miss; 
}

void UDefenceMinigame::HandleScore()
{
	EMinigameThreshold Result = CheckBar();

	switch (Result)
	{
	case EMinigameThreshold::Bad:
			DebugHelper::LogSuccess("33% damage reduction");
			break;
		case EMinigameThreshold::Good:
			DebugHelper::LogSuccess("66% reduction");
			break;
		case EMinigameThreshold::Perfect:
			DebugHelper::LogSuccess("Perfect parry");
		break;
		default:
			DebugHelper::LogSuccess("You suck! miss");
			break;
	}
}


void UDefenceMinigame::MoveSlider(const FVector2D& Position)
{
	checkf(Slider, TEXT("Slider in UDefenceMinigame::MoveSlider is null"));

	if (bStopSlider)
	{
		return;
	}
	
	FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;
	CurrentPosition.Y = 0;
	FVector2D DeltaMove = Position * Speed * GetWorld()->GetDeltaSeconds();
	FVector2D NewPosition = CurrentPosition + DeltaMove;
	NewPosition.Y = 0;
	const FVector2D LeftBarrierPosition = LeftSeparator->GetRenderTransform().Translation;

	if (const float Distance = FVector2D::Distance(LeftBarrierPosition , CurrentPosition); Distance >= DistanceThreshold)
	{
		return;
	}
	
	Slider->SetRenderTranslation(NewPosition);
}
