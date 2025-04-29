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
	/**
	 * Number of moves AI can make during it's turn (not done yet)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Moves")
	float Moves = 1.0f;

	/**
	 * Minigame power
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Moves")
	float MinigamePower = 1.0f;

	/**
	 * Modifier
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Actions")
	int32 ActionPointsModifier = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Actions")
	int32 WeaknessModifier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Actions")
	int32 MovePower = 1.0f;

	/**
	 * Used to restore AI health through 'Heal' Task
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parameters")
	float HealingPoint = 5.0f;
};
