// Fill out your copyright notice in the Description page of Project Settings.

#include "FBattleMemory.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

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
	//InBattleBullets = Bullets;

	if (Bullets.IsEmpty())
	{
		DebugHelper::LogMessage(8, FColor::White, "Bullets array passed at Battle Memory load is empty");
	}
	
	for (auto Pair : Bullets)
	{
		InBattleBullets.Add(Pair.Key, Pair.Value.Clone());
	}
	
}

void FBattleMemory::Clear()
{
	LastStoredQueue.Empty();
	InBattleBullets.Empty();
	EmotionsSpawned.Empty();
	EmotionsSpawnedClasses.Empty();
	Locations.Empty();
	Orientations.Empty();
}

