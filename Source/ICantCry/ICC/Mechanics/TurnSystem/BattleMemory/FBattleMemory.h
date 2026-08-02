// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/Bullet/BulletData.h"
#include "ICantCry/ICC/Actors/Bullet/Bullet.h"
#include "FBattleMemory.generated.h"

class AICC_Actor;
class AMob;

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
	TMap<TEnumAsByte<EBulletType>, FBullet> InBattleBullets;
	
	UPROPERTY()
	TArray<FVector> Locations;
	
	UPROPERTY()
	TArray<FRotator> Orientations;

	UPROPERTY() bool bBattleRetried = false;
	
	FBattleMemory();

	void Register(TSubclassOf<AMob> EmotionClass, const FVector& Location, const FRotator& Orientation);
	void Load(const TArray<AICC_Actor*>& Queue, const TMap<TEnumAsByte<EBulletType>, FBullet>& Bullets);
	void Clear();
	
	void ResetEmotionsStats();
	
};

