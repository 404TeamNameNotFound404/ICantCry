// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinigameUserWidget.generated.h"

UENUM(BlueprintType)
enum EMinigameThreshold
{
	Bad,
	Good,
	Perfect,
	Miss
};

/**
 * Mother class of the minigame
 */
UCLASS()
class ICANTCRY_API UMinigameUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetStopSlider(const bool& Stop);
	virtual EMinigameThreshold  CheckBar();
	
protected:
	UPROPERTY()
	bool bStopSlider = false;

	
};
