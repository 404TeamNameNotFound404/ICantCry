#pragma once
#include "CoreMinimal.h"
#include "EmotionMemory.generated.h" 

USTRUCT(BlueprintType)
struct FEmotionMemory
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Emotion Memory")
	FVector DefaultBattleLocation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Emotion Memory")
	FRotator DefaultBattleOrientation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Emotion Memory")
	FVector AttackLocation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Emotion Memory")
	float InitialDefencePower = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Emotion Memory")
	float InitialAttackPower = 0.0f;

	FEmotionMemory(); 
};
