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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	float AtkPower = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly ,Category="Stats")
	float DefPower = 1.f;

	UPROPERTY()
	int32 DbgDamageDealt;
	
	UPROPERTY()
	int32 DbgDamageTaken;
	
	FEmotionStat();
};

