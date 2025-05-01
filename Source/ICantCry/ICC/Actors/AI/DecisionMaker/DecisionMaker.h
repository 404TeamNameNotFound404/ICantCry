// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DecisionMaker.generated.h"

UENUM()
enum class EDecision : uint8
{
	HealItSelf UMETA(DisplayName = "Heal Itself"),
	HealOther UMETA(DisplayName = "Heal Other"),
	FreezedUp UMETA(DisplayName = "Freezed Up"),
	BuffItSelf UMETA(DisplayName = "Buff Itself"),
	BuffOther UMETA(DisplayName = "Buff Other"),
	DebuffShieldItSelf UMETA(DisplayName = "Debuff Shield Itself"),
	DebuffShieldOther UMETA(DisplayName = "Debuff Shield Other"),
	EnvyBurned UMETA(DisplayName = "Envy Burned"),
	None UMETA(DisplayName = "Attack"),
};

/**
 * Aleatory
 */
USTRUCT(BlueprintType)
struct FDecisionMaker
{
	GENERATED_BODY()

	FDecisionMaker();
	/**
	 * Plan the next decision based of probability and Aleatory math
	 * Each Mob turn before performing the move, AI will calculate the chance
	 * of performing special moves according to an aleatory value between 0 and 1
	 * and plan the next move according to it
	 * @return Decision
	 */
	EDecision Thought();

	TMap<EDecision, float> DecisionMap;

};

