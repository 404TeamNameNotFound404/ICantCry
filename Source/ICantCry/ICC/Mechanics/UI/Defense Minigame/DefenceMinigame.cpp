// Fill out your copyright notice in the Description page of Project Settings.
#include "DefenceMinigame.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "Kismet/GameplayStatics.h"


void UDefenceMinigame::NativeConstruct()
{
	Super::NativeConstruct();
}

EMinigameThreshold UDefenceMinigame::CheckBar()
{
	const float DistanceToFirst = FVector2D::Distance(Slider->GetRenderTransform().Translation,
	                                                  WorseScore->GetRenderTransform().Translation);
	const float DistanceToSecond = FVector2D::Distance(Slider->GetRenderTransform().Translation,
	                                                   MediumScore->GetRenderTransform().Translation);
	const float DistanceToParry = FVector2D::Distance(Slider->GetRenderTransform().Translation,
	                                                  PerfectScore->GetRenderTransform().Translation);

	if (DistanceToParry >= PerfectThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("FFAB5B"), "Perfect threshold hit");
		return EMinigameThreshold::Perfect;
	}
	if (DistanceToSecond >= GoodThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("4F1C51"), "Good threshold hit");
		return EMinigameThreshold::Good;
	}
	if (DistanceToFirst >= BadThreshold)
	{
		DebugHelper::LogMessage(3, FColor::FromHex("88304E"), "Bad threshold hit");
		return EMinigameThreshold::Bad;
	}

	return EMinigameThreshold::Miss;
}

void UDefenceMinigame::HandleScore()
{
	const EMinigameThreshold Result = CheckBar();
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	checkf(Instance, TEXT("Instance not found UDefenceMinigame::HandleScore()"));

	switch (Result)
	{
	case EMinigameThreshold::Bad:
		DebugHelper::LogMessage(3, FColor::FromHex("640D5F"), "33% damage reduction");
		Instance->GetPlayerStats()->MinigameModifier = 0.70f;;
		AMob::DealDamage();
		break;
	case EMinigameThreshold::Good:
		DebugHelper::LogMessage(3, FColor::FromHex("D91656"), "66% reduction");
		Instance->GetPlayerStats()->MinigameModifier = 0.33f;
		AMob::DealDamage();
		break;
	case EMinigameThreshold::Perfect:
		DebugHelper::LogMessage(3, FColor::FromHex("EB5B00"), "Perfect parry");
		Instance->GetPlayerStats()->MinigameModifier = 0.0f;
		AMob::DealDamage();
		break;
	default:
		DebugHelper::LogError("You suck! miss");
		Instance->GetPlayerStats()->MinigameModifier = 1.0f;
		AMob::DealDamage();
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

	if (const float Distance = FVector2D::Distance(LeftBarrierPosition, CurrentPosition); Distance >= DistanceThreshold)
	{
		return;
	}

	Slider->SetRenderTranslation(NewPosition);
}
