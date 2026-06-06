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
    
    const FGeometry& FirstGeo = FirstThreshold->GetCachedGeometry();
    const float FirstLeft = FirstGeo.GetAbsolutePosition().X;
    const float FirstWidth = FirstGeo.GetAbsoluteSize().X;

    const FGeometry& SecondGeo = SecondThreshold->GetCachedGeometry();
    const float SecondLeft = SecondGeo.GetAbsolutePosition().X;
    const float SecondWidth = SecondGeo.GetAbsoluteSize().X;

    if (SliderWidth <= 0.0f) return EMinigameThreshold::Bad;
	
    auto NormalizeX = [&](float X) { return (X - SliderLeft) / SliderWidth; };

    const float FirstNormLeft = NormalizeX(FirstLeft);
    const float FirstNormRight = NormalizeX(FirstLeft + FirstWidth);

    const float SecondNormLeft = NormalizeX(SecondLeft);
    const float SecondNormRight = NormalizeX(SecondLeft + SecondWidth);
	
    if (SliderThreshold >= SecondNormLeft && SliderThreshold <= SecondNormRight)
    {
    	DebugHelper::LogSuccess("RESULT: PERFECT");
        return EMinigameThreshold::Perfect;
    }

    if (SliderThreshold >= FirstNormLeft && SliderThreshold <= FirstNormRight)
    {
        DebugHelper::LogSuccess("RESULT: GOOD");
        return EMinigameThreshold::Good;
    }

	DebugHelper::LogError("RESULT: BAD");
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
		break;
	case Good:
		Instance->GetRuntimeStats().MinigameModifier = 1.f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		break;
	case Perfect:
		Instance->GetRuntimeStats().MinigameModifier = 1.5f;
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		
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
	// float Percentage = Slider->GetPercent();
	//
	// MinigameResult = CheckBar();
	//
	//
	// if (ScrollValue > 0)
	// {
	// 	if ( !Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
	// 	{
	// 		Percentage += ScrollValue * GetWorld()->GetDeltaSeconds() * SliderSpeed;
	// 		Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
	// 		Slider->SetPercent(Percentage);
	// 	}
	// 	
	// 	else if (Instance->GetCurrentPlayer()->GetBinder()->GetDecreaseMinigameScrollValue())
	// 	{
	// 		Percentage -= GetWorld()->GetDeltaSeconds() * DecreaseSpeed;
	// 		Percentage = FMath::Clamp(Percentage, 0.f, 1.f);
	// 		Slider->SetPercent(Percentage);
	// 	}
	// }
	//
	// TimerLeft += GetWorld()->GetDeltaSeconds() * TimerSpeed;
	//
	// if (TimerLeft >= TimeToDie)
	// {
	// 	UBattleHUD* Hud = Instance->GetCurrentPlayer()->GetBattleHUD();
	// 	Hud->EnableButtonsAfterShooting();
	// 	Hud->UpdateAp();
	// 	Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
	// 	HandleScore();
	// 	Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
	// 	
	// 	Hud->GetBattleHandler()->Fire(Hud->GetSelectedActor()->GetActorLocation(), Hud->GetCurrentBulletData()->DisplayColor);
	// 	Hud->GetBattleHandler()->SimulateHurt(Hud->GetCurrentBulletData()->DisplayColor);
	// }
	
	float Percentage = Slider->GetPercent();
	
	EMinigameThreshold CurrentFrameResult = CheckBar();
	
	if (CurrentFrameResult == EMinigameThreshold::Perfect)
	{
		MinigameResult = EMinigameThreshold::Perfect;
	}
	else if (CurrentFrameResult == EMinigameThreshold::Good && MinigameResult != EMinigameThreshold::Perfect)
	{
		MinigameResult = EMinigameThreshold::Good;
	}
	
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

