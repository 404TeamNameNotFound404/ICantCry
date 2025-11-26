// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DecisionTable.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FDecisionWeight
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float AttackWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BuffAtkWeight = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float DebuffAtkWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float EnvyBurnedWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float AshamedWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float HealOtherWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float HealWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float LowHealthWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float DebuffDefWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BuffDefWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BuffOtherAtkWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float BuffOtherDefWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float ShieldWeight = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float ShieldOtherWeight= 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float FreezedUpWeight = 0.f;
};

/**
 * Decision table to compile for each emotion to gave them the new chance when buffed
 */
UCLASS(Blueprintable)
class ICANTCRY_API UDecisionTable : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * No Buff/Debuff infliction 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight Idle;

	/**
	 * Buff atk chances alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight BuffAtk;

	/**
	 * Buff Def chances alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight BuffDef;

	/**
	 * Debuf atk chances alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight DebuffAtk;

	/**
	 * Debuff Def alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight DebuffDef;

	/**
	 * Envy burned chances alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight EnvyBurned;

	/**
	 * Ashamed chances alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight Ashamed;

	/**
	 * Debuff shield chance alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight DebuffShield;
	
	/**
	 * Low health alteration
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table")
	FDecisionWeight LowHealth;
};
