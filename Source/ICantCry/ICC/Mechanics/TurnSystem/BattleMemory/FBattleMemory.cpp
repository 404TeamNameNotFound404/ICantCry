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

