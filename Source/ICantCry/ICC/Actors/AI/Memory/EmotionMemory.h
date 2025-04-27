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

	FEmotionMemory(); 
};
