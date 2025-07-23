// Fill out your copyright notice in the Description page of Project Settings.

#include "FBattleMemory.h"

FBattleMemory::FBattleMemory()
{
	
}

void FBattleMemory::Register(TSubclassOf<AMob> EmotionClass, const FVector& Location, const FRotator& Orientation)
{
	EmotionsSpawnedClasses.Add(EmotionClass);
	Locations.Add(Location);
	Orientations.Add(Orientation);
}

void FBattleMemory::Load(const TArray<AICC_Actor*>& Queue, const TMap<TEnumAsByte<EBulletType>, FBullet>& Bullets)
{
	LastStoredQueue = Queue;
	InBattleBullets = Bullets;
}

void FBattleMemory::Clear()
{
	LastStoredQueue.Empty();
	InBattleBullets.Empty();
	EmotionsSpawned.Empty();
	EmotionsSpawnedClasses.Empty();
}

