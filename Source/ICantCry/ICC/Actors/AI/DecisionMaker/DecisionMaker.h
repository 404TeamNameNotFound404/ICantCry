// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "DecisionMaker.generated.h"

class UUBTTask_DefaultAtk;

UENUM()
enum class EDecision : uint8
{
	HealItSelf UMETA(DisplayName = "Heal Itself"),
	HealOther UMETA(DisplayName = "Heal Other"),
	FreezedUp UMETA(DisplayName = "Freezed Up"),
	BuffItSelf UMETA(DisplayName = "Buff Atk Itself"),
	BuffOther UMETA(DisplayName = "Buff Atk Other"),
	DebuffDefence UMETA(DisplayName = "Debuff Defence"),
	BuffDefence UMETA(DisplayName = "Buff Defence"),
	BuffOtherDefence UMETA(DisplayName = "Buff OtherDefence"),
	DebuffAtk UMETA(DisplayName = "Debuff Atk"),
	DebuffShieldItSelf UMETA(DisplayName = "Debuff Shield Itself"),
	DebuffShieldOther UMETA(DisplayName = "Debuff Shield Other"),
	EnvyBurned UMETA(DisplayName = "Envy Burned"),
	None UMETA(DisplayName = "Attack"),
	Invalid UMETA(DisplayName = "Invalid")
};


/**
 * Handles the priorities during decision process
 */
USTRUCT(BlueprintType)
struct FDecisionPriorities
{
	GENERATED_BODY()

	FDecisionPriorities(){}

	UPROPERTY() int32 BuffAtk = 0;
	UPROPERTY() int32 DebuffAtk = 1;
	UPROPERTY() int32 BuffDef = 1;
	UPROPERTY() int32 DebuffDef = 2;
	UPROPERTY() int32 LowHealth = 2;
	UPROPERTY() int32 EnvyBurned = 3;
	UPROPERTY() int32 Ashamed = 4;
	UPROPERTY() int32 DebuffShield = 4;
	UPROPERTY() int32 FallBack = 5; // just in case jump to attack


	int32 GetPriority(const EDecision& Decision) const;
	EDecision PickHighestDecision(const TArray<EDecision>& Decisions) const;
};

/**
 * Aleatory
 */
USTRUCT(BlueprintType)
struct FDecisionMaker
{
	GENERATED_BODY()

	FDecisionMaker();

	void Setup(AMob* Current);
	
	/**
	 * Plan the next decision based of probability and Aleatory math
	 * Each Mob turn before performing the move, AI will calculate the chance
	 * of performing special moves according to an aleatory value between 0 and 1
	 * and plan the next move according to it
	 * @return Decision
	 */
	EDecision Thought();


	/**
	 * Attempt to Think a different decision according to mob's priorities, and it's current buffs/debuffs
	 * @return Decision
	 */
	EDecision EnhancedThought(AMob* Emotion);
	
	TMap<EDecision, float> DecisionMap;

	/**
	 * Clear the Decision Map in order to avoid repetitions
	 */
	void Clear();

	/**
	 * Clear the last decision after the end of the turn
	 */
	void ResetDecision();

private:
	UPROPERTY()
	EDecision LastDecision = EDecision::None;

	bool CanRepeat(const EDecision& Decision);

};

