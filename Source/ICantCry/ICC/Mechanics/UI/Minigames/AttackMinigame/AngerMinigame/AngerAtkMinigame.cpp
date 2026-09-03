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
	
	if (DebugHelper::IsGamepadPlugged())
	{
		ButtonTexture->SetBrushFromTexture(ButtonTextures[1]);
		ButtonTexture->SetDesiredSizeOverride({50,50});
	}
	else
	{
		ButtonTexture->SetBrushFromTexture(ButtonTextures[0]);
		ButtonTexture->SetDesiredSizeOverride({50,50});
	}
}

EMinigameThreshold UAngerAtkMinigame::CheckBar()
{
	const float SliderThreshold = Slider->GetPercent();
    const FGeometry& SliderGeo = Slider->GetCachedGeometry();
	
    const float SliderLeft = SliderGeo.GetAbsolutePosition().X;
    const float SliderWidth = SliderGeo.GetAbsoluteSize().X; 
 
    if (SliderWidth <= 0.0f) return EMinigameThreshold::Bad;

	
    if (SliderThreshold >= 0.85f)
    {
        return EMinigameThreshold::Perfect;
    }
 
    if (SliderThreshold >= 0.64f && SliderThreshold <= 0.70)
    {
        return EMinigameThreshold::Good;
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
		{
			Instance->GetRuntimeStats().MinigameModifier = 0.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			Instance->GetPlayerStats()->RuntimeStats.ApModifier = 1;
		}break;
	case Good:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			Instance->GetPlayerStats()->RuntimeStats.ApModifier = 1;
		}break;
	case Perfect:
		{
			Instance->GetRuntimeStats().MinigameModifier = 1.5f;
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			Instance->GetPlayerStats()->RuntimeStats.ApModifier = 1;
		
			DebugHelper::LogMessage(9, FColor::Black, "Perfect anger minigame score");
		}break;
	}
	
	DebugHelper::LogMessage(10, FColor::Blue, "Returned " + GetMinigameScore(MinigameResult));
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
	Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
}

void UAngerAtkMinigame::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	
	float Percentage = Slider->GetPercent();
	Percentage -= GetWorld()->GetDeltaSeconds() * DecreaseSpeed;
	Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
	Slider->SetPercent(Percentage);
	Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
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
		// if (!Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
		// {
		// 	Percentage += ScrollValue * GetWorld()->GetDeltaSeconds() * SliderSpeed;
		// 	Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
		// 	Slider->SetPercent(Percentage);
		// }
		
		Percentage += ScrollValue * GetWorld()->GetDeltaSeconds() * IncreaseSpeed;
		Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
		Slider->SetPercent(Percentage);
		
		
		if (!GetWorld()->GetTimerManager().IsTimerActive(Smash))
		{
			GetWorld()->GetTimerManager().SetTimer(Smash, [&]
			{
			   Percentage -= GetWorld()->GetDeltaSeconds() * DecreaseSpeed;
			   Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
			   Slider->SetPercent(Percentage);
			   Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
			   ScrollValue = 0;
			}, SmashRate, false);
		}
	}
	
	TimerLeft += GetWorld()->GetDeltaSeconds() * TimerSpeed;
	
	if (TimerLeft >= TimeToDie)
	{
		UBattleHUD* Hud = Instance->GetCurrentPlayer()->GetBattleHUD();
		Hud->EnableButtonsAfterShooting();
		Hud->UpdateAp();
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		HandleScore();
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
		
		Hud->GetBattleHandler()->Fire(Hud->GetSelectedActor()->GetActorLocation(), Hud->GetCurrentBulletData()->DisplayColor);
		Hud->GetBattleHandler()->SimulateHurt(Hud->GetCurrentBulletData()->DisplayColor);
	}
}

