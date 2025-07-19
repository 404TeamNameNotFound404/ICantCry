// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "UObject/Object.h"
#include "DropSystem.generated.h"

class UVictoryVisualizer;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UDropSystem : public UObject
{
	GENERATED_BODY()

public:

	void Drop(UWorld* World, UVictoryVisualizer* VictoryVisualizer, const TArray<AICC_Actor*>& Queue);
};
