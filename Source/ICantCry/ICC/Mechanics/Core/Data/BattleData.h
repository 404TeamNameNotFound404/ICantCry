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
