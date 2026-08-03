// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "FIccBattleDebuggerData.generated.h"

class AICC_Actor;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FIccBattleDebuggerData
{
	GENERATED_BODY()
	
	UPROPERTY() FString TargetName;
	UPROPERTY() float Health;
	UPROPERTY() bool bIsBuffed;
	UPROPERTY() bool bIsDebuffed;
	UPROPERTY() bool bIsAfflicted;
	UPROPERTY() int32 BuffCounter = -1;
	UPROPERTY() int32 DebuffCounter = -1;
	UPROPERTY() int32 StatusCounter = -1;
	UPROPERTY() FString CurrentBuff;
	UPROPERTY() FString CurrentDebuff;
	UPROPERTY() FString CurrentMalus;
	
	void RefreshInfo(AICC_Actor* Target);
};


USTRUCT(BlueprintType)
struct FIccDebuggerDecisionData
{
	GENERATED_BODY()
	
	UPROPERTY() FString TargetName;
	UPROPERTY() FString DecisionPicked;
	UPROPERTY() FString DecisionTable;
	
	void RefreshInfo(AICC_Actor* Target);
};


USTRUCT(BlueprintType)
struct FIccDebuggerTrackingData
{
	GENERATED_BODY()
	
	UPROPERTY() FString TargetName;
	UPROPERTY() FString CurrentBuff;
	UPROPERTY() FString CurrentDebuff;
	UPROPERTY() float AttackPower;
	UPROPERTY() float DefencePower;
	UPROPERTY() float DamageDealt;
	
	void RefreshInfo(AICC_Actor* Target);
};