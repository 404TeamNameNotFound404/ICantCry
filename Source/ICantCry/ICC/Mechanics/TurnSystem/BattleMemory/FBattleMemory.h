// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "FBattleMemory.generated.h"

USTRUCT(BlueprintType)
struct FBattleMemory
{
	GENERATED_BODY();

	UPROPERTY()
	TArray<AICC_Actor*> LastStoredQueue;

	UPROPERTY()
	TArray<TSubclassOf<AMob>> EmotionsSpawnedClasses;

	UPROPERTY()
	TArray<AMob*> EmotionsSpawned;
	
	UPROPERTY()
	TArray<FVector> Locations;
	
	UPROPERTY()
	TArray<FRotator> Orientations;

	FBattleMemory();

	void Register(TSubclassOf<AMob> EmotionClass, const FVector& Location, const FRotator& Orientation);
	
};

