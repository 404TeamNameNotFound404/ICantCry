// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "RuntimeStats.generated.h"

USTRUCT(Blueprintable)
struct FRuntimeStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	int32 AttackPower = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	int32 DefencePower = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MaxExperience = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Experience = 0.0f;

	/**
	 * Display the total Exp gained after battle if won
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Stats")
	float ExpSummary = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float ApModifier = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float MinigameModifier = 0.5f;

	UPROPERTY()
	FDamage Stats;
	
	UPROPERTY()
	int32 DbgDamageTaken;
	
	UPROPERTY()
	int32 DbgDamageDealt;
	
	void Reset(const float& InHealth);
};