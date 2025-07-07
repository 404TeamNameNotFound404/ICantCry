// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerStats.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UPlayerStats : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Current Player Level
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Level = 0;
	/**
	 * Level Cap , Assign it yourself
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 LevelCap;
	/**
	 * Actual Experience
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Experience;

	/**
	 * Read-only property used for player progress bar
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	float MaxHealth = 50;

	/**
	 * Current Health
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float CurrentHealth = MaxHealth;
	
	/**
	 * Attack Power
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	int32 AttackPower;
	/**
	 * Defence Power
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	int32 DefencePower;
	/**
	 * Battle Fast forward 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	float BattleSpeedFlow = 1.0f;
	
	/**
	 * Player status (if affected by any)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	FName Status;


	/**
 * Minigame modifier 
 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float MinigameModifier = 0.5f;

	/**
	 * Ap modifier
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float ApModifier = 1.0f;

	/**
 * Weakness modifier
 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float WeaknessModifier = 0.0f;

	
};
