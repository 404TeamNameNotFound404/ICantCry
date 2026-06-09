// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "AttackMinigame.generated.h"

/**
 * Attack Minigame
 */
UCLASS(Blueprintable)
class ICANTCRY_API UAttackMinigame : public UMinigameUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	virtual void MoveSlider(const FVector2D& Position) override;
	virtual EMinigameThreshold CheckBar() override;


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float Speed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float DangerThresholdLeft;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float DangerThresholdRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float SafeAreaThresholdLeft;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float SafeAreaThresholdRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float PerfectAreaThresholdLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float PerfectAreaThresholdRight;

	virtual void HandleScore() override;

	UPROPERTY(meta = (BindWidget))
	UImage* Slider;
	UPROPERTY(meta = (BindWidget))
	UImage* DangerBorderLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* DangerBorderRight;
	UPROPERTY(meta = (BindWidget))
	UImage* SafeAreaLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* SafeAreaRight;
	UPROPERTY(meta = (BindWidget))
	UImage* PerfectAreaLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* PerfectAreaRight;

	UPROPERTY()
	FVector2D StartingSliderPosition;

	UPROPERTY()
	int MovementDirection = 1;

	UPROPERTY()
	float BorderLeft = 0.0f;

	UPROPERTY()
	float BorderRight = 950.0f;

	UPROPERTY()
	UICantCryGameInstance* GameInstance;
	
	UPROPERTY()
	TEnumAsByte<EMinigameThreshold> T;
};
