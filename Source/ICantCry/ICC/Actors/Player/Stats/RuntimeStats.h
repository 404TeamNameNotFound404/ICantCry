// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeStats.generated.h"

USTRUCT(Blueprintable)
struct FRuntimeStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	int32 AttackPower = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	int32 DefencePower = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Experience = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float ApModifier = 1.0f;
};