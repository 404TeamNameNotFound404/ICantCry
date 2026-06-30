// Fill out your copyright notice in the Description page of Project Settings.
#include "DefenceMinigame.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Kismet/GameplayStatics.h"


void UDefenceMinigame::NativeConstruct()
{
	Super::NativeConstruct();
	
	UTexture2D* InputIcon;
	
	if (DebugHelper::IsGamepadPlugged())
	{
		InputIcon = Cast<UICantCryGameInstance>(GetGameInstance())->GetIconMap()["OPad_X"];
	}
	else
	{
		InputIcon = Cast<UICantCryGameInstance>(GetGameInstance())->GetIconMap()["OKey_Spacebar"];
	}
	
	Icon->SetBrushFromTexture(InputIcon);
	const FVector2D TextureSize = FVector2D(InputIcon->GetSurfaceWidth(), InputIcon->GetSurfaceHeight());
	Icon->SetDesiredSizeOverride(TextureSize);
	
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	Instance->GetRuntimeStats().MinigameModifier = 1.0f;
}

EMinigameThreshold UDefenceMinigame::CheckBar()
{
	const float DistanceToFirst = FVector2D::Distance(Slider->GetRenderTransform().Translation.GetAbs(),
	                                                 	WorseScore->GetRenderTransform().Translation.GetAbs());
	const float DistanceToSecond = FVector2D::Distance(Slider->GetRenderTransform().Translation.GetAbs(),
	                                                   MediumScore->GetRenderTransform().Translation.GetAbs());
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
	
	switch (Result)
	{
	case EMinigameThreshold::Bad:
		DebugHelper::LogMessage(3, FColor::FromHex("640D5F"), "33% damage reduction");
		DebugHelper::AddMessageToLog("[Defence Minigame]: 33% damage reduction");
		Instance->GetRuntimeStats().MinigameModifier = 0.66f; // previously 0.70
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion()->InflictDamage();
		break;
	case EMinigameThreshold::Good:
		DebugHelper::LogMessage(3, FColor::FromHex("D91656"), "66% reduction");
		DebugHelper::AddMessageToLog("[Defence Minigame]: 66% damage reduction");
		Instance->GetRuntimeStats().MinigameModifier = 0.33f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion()->InflictDamage();
		break;
	case EMinigameThreshold::Perfect:
		DebugHelper::LogMessage(3, FColor::FromHex("EB5B00"), "Perfect parry");
		DebugHelper::AddMessageToLog("[Defence Minigame]: Perfect parry");
		Instance->GetRuntimeStats().MinigameModifier = 0.0f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion()->InflictDamage();
		break;
	default:
		DebugHelper::LogError("You suck! miss");
		DebugHelper::AddMessageToLog("[Defence Minigame]: You suck! miss");
		Instance->GetRuntimeStats().MinigameModifier = 1.0f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion()->InflictDamage();
		break;
	}
}

FString UDefenceMinigame::GetThresholdName(const EMinigameThreshold& T) const
{
	switch (T)
	{
	case Bad:
		return "Bad";
	case Good:
		return "Good";
	case Perfect:
		return "Perfect";
	case Miss:
		return "Miss";
	default:
		return "You suck!";
	}
}


void UDefenceMinigame::MoveSlider(const FVector2D& Position)
{
	checkf(Slider, TEXT("Slider in UDefenceMinigame::MoveSlider is null"));

	if (bStopSlider)
	{
		return;
	}
	
	auto Bar = CheckBar();
	
	FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;
	CurrentPosition.Y = 0;
	FVector2D DeltaMove = Position * Speed * GetWorld()->GetDeltaSeconds();
	FVector2D NewPosition = CurrentPosition + DeltaMove;
	NewPosition.Y = 0;
	const FVector2D LeftBarrierPosition = LeftSeparator->GetRenderTransform().Translation;
	
	if (const float Distance = FVector2D::Distance(LeftBarrierPosition , CurrentPosition); Distance >= EndThreshold)
	{
		Instance = Cast<UICantCryGameInstance>(GetGameInstance());
		checkf(Instance, TEXT("Instance not found UDefenceMinigame::HandleScore()"));
		this->RemoveFromParent();
		Instance->GetPlayerStats()->MinigameModifier = 1.0f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion()->InflictDamage();
		AMob::MinigameEnded = true;
		AMob::SetMinigameStarted(false);
		DebugHelper::LogError("You hit late!");
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
		return;
	}
	
	Slider->SetRenderTranslation(NewPosition);
}
