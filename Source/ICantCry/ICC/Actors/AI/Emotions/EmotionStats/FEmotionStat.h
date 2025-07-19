#pragma once

#include "CoreMinimal.h"
#include "FEmotionStat.generated.h"

USTRUCT(BlueprintType)
struct FEmotionStat
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, Category="Stats")
	float Health;

	UPROPERTY(VisibleInstanceOnly, Category="Stats")
	bool bAlive;

	FEmotionStat();
};

