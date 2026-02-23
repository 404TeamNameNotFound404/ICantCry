// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "CurlingMinigame.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UCurlingMinigame : public UMinigameUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void MoveSlider(const FVector2D& Position) override;
	virtual void HandleScore() override;
	virtual EMinigameThreshold CheckBar() override;
	virtual void Flow() override;
	
	// Bar Setup
	UPROPERTY(meta=(BindWidget)) UImage* NotBadFrontierBottom; // Blue border bottom
	UPROPERTY(meta=(BindWidget)) UImage* NotBadFrontierUp; // Blue border up
	UPROPERTY(meta=(BindWidget)) UImage* GoodFrontierUp; // White border up
	UPROPERTY(meta=(BindWidget)) UImage* GoodFrontierBottom; // White border bottom
	UPROPERTY(meta=(BindWidget)) UImage* PerfectFrontierUp; // Red border up
	UPROPERTY(meta=(BindWidget)) UImage* PerfectFrontierBottom; // Red border bottom
	
	// Curling Field Setup
	UPROPERTY(meta=(BindWidget)) UImage* BlueFieldSide;
	UPROPERTY(meta=(BindWidget)) UImage* WhiteFieldSide;
	UPROPERTY(meta=(BindWidget)) UImage* RedFieldSide;
	
	UPROPERTY(meta=(BindWidget)) UImage* KeyIcon;
	UPROPERTY(meta=(BindWidget)) UImage* Ball;
	
	UPROPERTY(meta=(BindWidget)) UProgressBar* Slider;
	
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* BlueBallPos;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* WhiteBallPos;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* RedBallPos;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame")
	float SliderSpeed = 1.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame")
	float DecreaseSpeed = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame")
	float BallSpeed = 1.8f;
	
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame") float BadScoreRange = 500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame") float GoodScoreRange = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category="Minigame") float PerfectScoreRange = 40.f;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TMap<FName, UTexture2D*> IconMap;
	
	UPROPERTY()
	TEnumAsByte<EMinigameThreshold> MinigameResult;
	
	UPROPERTY()
	UICantCryGameInstance* Instance;
	
	UPROPERTY() FVector2D BallStartingPosition;
	UPROPERTY() FVector2D BallEndingPosition;
	UPROPERTY() FVector2D BallTargetEndingPoint;
	
	float GetPercentFromImage(UImage* Image);
};
