// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackMinigame.h"

#include "Components/PanelWidget.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

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
    const FVector2D CurrentPosition = Slider->GetCachedGeometry().GetAbsolutePosition();
    
    const float X = CurrentPosition.X;
    
    const float LeftDangerX = DangerBorderLeft->GetCachedGeometry().GetAbsolutePosition().X;
    const float LeftSafeX = SafeAreaLeft->GetCachedGeometry().GetAbsolutePosition().X;
    const float PerfectLeftX = PerfectAreaLeft->GetCachedGeometry().GetAbsolutePosition().X;
    const float PerfectRightX = PerfectAreaRight->GetCachedGeometry().GetAbsolutePosition().X;
    const float RightSafeX = SafeAreaRight->GetCachedGeometry().GetAbsolutePosition().X;
    const float RightDangerX = DangerBorderRight->GetCachedGeometry().GetAbsolutePosition().X;
	
	
    if (X >= PerfectLeftX && X <= PerfectRightX)
    {
        return EMinigameThreshold::Perfect;
    }
	
    if ((X >= LeftSafeX && X < PerfectLeftX) || (X > PerfectRightX && X <= RightSafeX))
    {
        return EMinigameThreshold::Good;
    }
	
    if ((X >= LeftDangerX && X < LeftSafeX) || (X > RightSafeX && X <= RightDangerX))
    {
        return EMinigameThreshold::Bad;
    }
	
    return EMinigameThreshold::Miss;
}



void UAttackMinigame::HandleScore()
{
	const EMinigameThreshold Result = CheckBar();
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	switch (Result)
	{
		case EMinigameThreshold::Bad:
			DebugHelper::LogError("Bad minigame score!");
		    Instance->GetPlayerStats()->MinigameModifier = 0.5f;
		    Instance->GetCurrentDamageData().CalculateDamage(true);
		    Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			break;
		case EMinigameThreshold::Good:
			DebugHelper::LogWarning("Good minigame score!");
		    Instance->GetPlayerStats()->MinigameModifier = 1.0f;
		    Instance->GetCurrentDamageData().CalculateDamage(true);
		    Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			break;
		case EMinigameThreshold::Perfect:
			DebugHelper::LogSuccess("Perfect minigame score!");
		    Instance->GetPlayerStats()->MinigameModifier = 1.5f;
		    Instance->GetCurrentDamageData().CalculateDamage(true);
		    Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			break;
		default:
			DebugHelper::LogMessage(3, FColor::FromHex("ADB2D4"),"Unknown minigame score!");
		    Instance->GetPlayerStats()->MinigameModifier = 0.5f;
		    Instance->GetCurrentDamageData().CalculateDamage(true);
		    Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
			break;
	}
}
