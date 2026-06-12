// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
class ICANTCRY_API UtilityFunctions 
{

public:
	static void LoadSceneByName(UWorld* World, const FName& Name);
	static void LoadBattleSceneRandom(UWorld* World, const TArray<FName>& BattleSceneRandomNames);
};
