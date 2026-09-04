// Fill out your copyright notice in the Description page of Project Settings.

#include "FBattleMemory.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

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

void FBattleMemory::ResetEmotionsStats()
{
	if (LastStoredQueue.IsEmpty()) return;
	
	for (AICC_Actor* Actor : LastStoredQueue)
	{
		if (!IsValid(Actor)) continue;
		if (Actor->IsA(AICC_Player::StaticClass())) continue;
		
		AMob* Emotion = Cast<AMob>(Actor);
		if (!Emotion) continue;
		
		Emotion->GetStats().AtkPower = Emotion->GetAIMemory().InitialAttackPower;
		Emotion->GetStats().DefPower = Emotion->GetAIMemory().InitialDefencePower;
		
		DebugHelper::AddMessageToLog("[Battle Memory]: " + Emotion->GetEmotionName() + 
			" resets it's atk power to " + FString::SanitizeFloat(Emotion->GetStats().AtkPower) + 
			" and def: " + FString::SanitizeFloat(Emotion->GetStats().DefPower));
	}
}

