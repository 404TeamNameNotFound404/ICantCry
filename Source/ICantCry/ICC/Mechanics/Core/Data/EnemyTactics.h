// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyTactics.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UEnemyTactics : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Moves")
	float Moves;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Moves")
	float MinigamePower = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Actions")
	int32 ActionPointsModifier = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Actions")
	int32 WeaknessModifier = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Actions")
	int32 MovePower = 1.0f;
};
