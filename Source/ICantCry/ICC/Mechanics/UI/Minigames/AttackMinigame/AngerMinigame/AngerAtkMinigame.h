// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "AngerAtkMinigame.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UAngerAtkMinigame : public UMinigameUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MoveSlider(const FVector2D& Position) override;
	virtual EMinigameThreshold CheckBar() override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void HandleScore() override;
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* Slider;
	
	UPROPERTY(meta=(BindWidget))
	UImage* FirstThreshold; // first are (good shoot)
	
	UPROPERTY(meta=(BindWidget))
	UImage* SecondThreshold; // second area (nice shoot)
	
	/**
	 * The current displayed icon, show the key or the gamepad button to press when minigame is started
	 */
	UPROPERTY(meta=(BindWidget))
	UImage* ButtonTexture; 
	
	UPROPERTY(meta=(BindWidget))
	UImage* TimerLeftIcon;
	
	UPROPERTY() FVector2D StartingSliderPosition;
	
	UPROPERTY() int32 MovementDirection = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigame", meta = (AllowPrivateAccess = "true"))
	float SliderSpeed = 1.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigame")
	float DecreaseSpeed = 0.8f;
	
	UPROPERTY()
	float TimerLeft = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigame", meta = (AllowPrivateAccess = "true"))
	float TimeToDie = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigame", meta = (AllowPrivateAccess = "true"))
	float TimerSpeed = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigame")
	FVector2D SliderLocationTarget = {0,0};
	
	UPROPERTY()
	UICantCryGameInstance* Instance = nullptr;
	
	UPROPERTY()
	TEnumAsByte<EMinigameThreshold> MinigameResult;
	
private:
	FString GetMinigameScore(const EMinigameThreshold& Value) const;
};
