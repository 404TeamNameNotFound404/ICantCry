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
	  
	FDamage() : BulletData(nullptr), PlayerStats(nullptr), AIMoves(nullptr) , EnemyData(nullptr) {};

	int CalculateDamage(const bool& IsPlayerAttacking);
};

/**
 * UObject container for damage calculation
 */
UCLASS()
class ICANTCRY_API UDamageCalculator : public UObject
{
	GENERATED_BODY()

public:
	UDamageCalculator();
	UDamageCalculator(UBulletData* Data, UPlayerStats* Stats , UEnemyDatas* EnemyData ,UEnemyTactics* Moves);

private:
	UPROPERTY()
	FDamage DamageMath;
};
