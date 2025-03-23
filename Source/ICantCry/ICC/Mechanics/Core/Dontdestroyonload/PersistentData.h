// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PersistentData.generated.h"

/**
 * Data container used to store player's data across levels in order to recreate player during scene
 * transitions.
 */
UCLASS(Blueprintable)
class ICANTCRY_API UPersistentData : public UDataAsset
{
	GENERATED_BODY()

public:
	// First of hall I think it can be good to store Position, Rotation and data such as AP, HP
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	FVector PlayerPosition;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	FRotator PlayerOrientation;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	float PlayerHp;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	float AbilityPoints;

	/*
	   UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Rewards")
	   RewardData* data probably
	*/
};
