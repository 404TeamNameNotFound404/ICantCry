// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
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
	
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TMap<FName, UTexture2D*> IconMap;
};
