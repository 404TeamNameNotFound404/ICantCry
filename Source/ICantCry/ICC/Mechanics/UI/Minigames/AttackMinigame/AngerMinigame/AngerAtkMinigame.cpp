// Fill out your copyright notice in the Description page of Project Settings.


#include "AngerAtkMinigame.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UAngerAtkMinigame::NativeConstruct()
{
	Super::NativeConstruct();
	Slider->SetPercent(0.0f);
	Slider->SetFillColorAndOpacity(FLinearColor::Red);
	
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
}

EMinigameThreshold UAngerAtkMinigame::CheckBar()
{
	const FGeometry& Geo = Slider->GetCachedGeometry();
	const float SliderThreshold = Geo.GetAbsolutePosition().Y;
	
	const float FirstLeft = FirstThreshold->GetCachedGeometry().GetAbsolutePosition().X;
	const float SecondLeft = SecondThreshold->GetCachedGeometry().GetAbsolutePosition().Y;
	
	const bool bInside = SliderThreshold >= FirstLeft && SliderThreshold <= SecondLeft;
	const bool bPastSecond = SliderThreshold > SecondLeft;
	const bool bBeginning = SliderThreshold < FirstLeft;
	
	if (bInside)
	{
		return EMinigameThreshold::Good;
	}
	
	if (bPastSecond)
	{
		return EMinigameThreshold::Perfect;
	}
	
	if (bBeginning)
	{
		return EMinigameThreshold::Bad;
	}
	
	return EMinigameThreshold::Bad;
}

void UAngerAtkMinigame::HandleScore()
{
	if (TimerLeft < TimeToDie)
	{
		return;
	}
	
	switch (MinigameResult)
	{
	default:
	case Miss:
	case Bad:
		Instance->GetRuntimeStats().MinigameModifier = 0.5f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}
		break;
	case Good:
		Instance->GetRuntimeStats().MinigameModifier = 1.f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->Type == EBulletType::Shame)
		{
			Cast<AMob>(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor())->GetStatusTracker()->InflictStatus(
				EAfflictedStatus::EAShame, Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor());
		}
		break;
	case Perfect:
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
	
	DebugHelper::LogMessage(10, FColor::Blue, "Returned " + GetMinigameScore(MinigameResult));
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
	Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
}

FString UAngerAtkMinigame::GetMinigameScore(const EMinigameThreshold& Value) const
{
	switch (Value)
	{
	default:
	case Bad:
		return "Bad";
	case Good:
		return "Good";
	case Perfect:
		return "Perfect";
	case Miss:
		return "Miss";
	}
}

void UAngerAtkMinigame::MoveSlider(const FVector2D& Position)
{
	float Percentage = Slider->GetPercent();
	
	MinigameResult = CheckBar();
	
	
	if (ScrollValue > 0)
	{
		if ( !Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
		{
			Percentage += ScrollValue * GetWorld()->GetDeltaSeconds() * SliderSpeed;
			Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
			Slider->SetPercent(Percentage);
		}
		
		else if (Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
		{
			Percentage -= GetWorld()->GetDeltaSeconds() * DecreaseSpeed;
			Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
			Slider->SetPercent(Percentage);
		}
	}
	
	TimerLeft += GetWorld()->GetDeltaSeconds() * TimerSpeed;
	
	if (TimerLeft >= TimeToDie)
	{
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		HandleScore();
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
	}
}

