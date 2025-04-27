// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDatas.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UEnemyDatas : public UDataAsset
{
	GENERATED_BODY()


public:
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EnemyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AbilityPoints = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefencePower = 1.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool Alive;

	// aggiungi potenza degli attacchi che non ho messo e che trovi nel documento che ti ho linkato e che trovi su generale che è ultimo link di drive 


	
};
