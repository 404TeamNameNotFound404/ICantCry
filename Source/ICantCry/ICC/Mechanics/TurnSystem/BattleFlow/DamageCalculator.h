// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "DamageCalculator.generated.h"


USTRUCT(BlueprintType)
struct FDamage
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UBulletData* BulletData;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UPlayerStats* PlayerStats;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UEnemyTactics* AIMoves;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UEnemyDatas* EnemyData;

	FDamage();

	/**
	 * Fills the struct with the corresponding data
	 * @param BData  Bullet Data
	 * @param PStats Player Stats
	 * @param AITactics AIMoves
	 * @param EData Enemy Data
	 */
	FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData);
	int CalculateDamage(const bool& IsPlayerAttacking);
};

