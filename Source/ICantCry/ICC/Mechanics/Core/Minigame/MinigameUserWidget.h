// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinigameHandler.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Actors/Bullet/BulletData.h"
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
	virtual void HandleScore();
	virtual void Flow();
	virtual bool OffsideChecker();
	
	void Handle(UBulletData* BulletData, AMinigameHandler* MinigameHandler);
	
	void SetScrollValue(const float& Value);
	float& GetScrollValue();
	
	UFUNCTION(BlueprintCallable)
	virtual void MoveSlider(const FVector2D& Position);
	
	
protected:
	UPROPERTY()
	bool bStopSlider = false;
	
	UPROPERTY()
	float ScrollValue = 0.f;
};
