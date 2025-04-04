// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "DamageCalculator.generated.h"


USTRUCT(BlueprintType)
struct FDamage
{
	GENERATED_BODY()
	/*
	 *  UPROPERTY()
	 * 	TArray<UBulletData*> Bullets;
	*/
	UPROPERTY()
	UPlayerStats* PlayerStats;
	
	  /*   UPROPERTY()
	  *    UEnemyTactics* Moves; 
	  */

	/* FDamage( UBulletData* BulletData, UPlayerStats* Stats, UEnemyTactics* EMoves) : BulletData(Bullets) , Stats(PlayerStats), EMoves(Moves) {} */
};

/**
 * UObject container for damage calculation
 */
UCLASS()
class ICANTCRY_API UDamageCalculator : public UObject
{
	GENERATED_BODY()

public:
	

private:
	UPROPERTY()
	FDamage DamageMath;
};
