// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "Components/ProgressBar.h"
#include "MobHealthBar.generated.h"

/**
 * Enemy Health bar
 */
UCLASS(Blueprintable)
class ICANTCRY_API UMobHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	float GetTotalHealth() const;
	float GetCurrentHealth() const;
	void SetCurrentHealth(const float& Variation);

	void FillData();

	UPROPERTY()
	UEnemyDatas* Info;

	void Restore(const float& RestoredHealth);
	bool IsLow() const;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY()
	float TotalHealth;

	UPROPERTY()
	float CurrentHealth;
	
};
