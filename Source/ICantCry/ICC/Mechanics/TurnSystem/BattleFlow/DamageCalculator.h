// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "DamageCalculator.generated.h"

class UICantCryGameInstance;
class UPlayerStats;
class AICC_Actor;

USTRUCT(BlueprintType)
struct FDamage
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UBulletData* BulletData = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UPlayerStats* PlayerStats = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UEnemyTactics* AIMoves = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UEnemyDatas* EnemyData = nullptr;

	UPROPERTY()
	UICantCryGameInstance* Instance;
	
	UPROPERTY() AICC_Actor* Self;

	FDamage();

	/**
	 * Fills the struct with the corresponding data
	 * @param BData  Bullet Data
	 * @param PStats Player Stats
	 * @param AITactics AIMoves
	 * @param EData Enemy Data
	 * @param SelfPtr Self Ptr
	 * @param GI GameInstance
	 */
	FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData, AICC_Actor* SelfPtr ,UICantCryGameInstance* GI = nullptr);
	int CalculateDamage(const bool& IsPlayerAttacking);
	
	
};