// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ICantCry/ICC/Actors/AI/MobType.h"
#include "ICantCry/ICC/Actors/AI/Emotions/EmotionStats/FEmotionStat.h"
#include "EnemyDatas.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UEnemyDatas : public UDataAsset
{
	GENERATED_BODY()


public:
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Runtime Stats")
	FEmotionStat RuntimeStats;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EnemyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AbilityPoints = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefencePower = 1.0f;

	/**
	 * Joy dedicated variable
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RestorePower = 1.0f;

	/**
	 * Set the exp each emotions give to player after battle
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ExpGiven = 1;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool Alive;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TEnumAsByte<EMobType> Type;

	
};
