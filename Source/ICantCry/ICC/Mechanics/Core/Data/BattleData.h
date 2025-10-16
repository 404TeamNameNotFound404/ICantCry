// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UBattleData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Battle Data")
	float BuffAtkIncrement = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Battle Data")
	float BuffDefIncrement = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Battle Data")
	float DebuffAtkMalus = 0.20f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Battle Data")
	float DebuffDefMalus = 0.20f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emotion Battle Data")
	float EmotionAtkBuffIncrement = 1.25f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emotion Battle Data")
	float EmotionDefBuffIncrement = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emotion Battle Data")
	float EmotionAtkDebuffMalus = 0.20f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emotion Battle Data")
	float EmotionDefDebuffMalus = 0.20f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float BuffAtkChance = 0.55f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float HealItselfChance = 0.10f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float HealOtherChance = 0.20f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float DebuffDefChance = 0.60f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float DebuffAtkChance = 0.60f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float BuffDefChance = 0.60f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float BuffOtherDefChance = 0.20f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float DebuffShieldItselfChance = 0.50f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float DebuffShieldOtherChance = 0.10f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float FreezedUpChance = 0.40f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float EnvyBurnedChance = 0.50f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float BuffOtherAtkChance = 0.20f;

	/**
	 * This variable is used just for calm and anger for debugging purposes
	 * (8 July 2025)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Task Chances")
	float NormalAttackChance = 0.5f;
	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Drop System")
	int AngerDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int JoyDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int FearDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int DisgustDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int AnxietyDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int SadnessDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int JealousyDropQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int CalmDropQuantity = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop System")
	int ShameDropQuantity = 1;
};
