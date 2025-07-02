// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BPRequirements.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UBPRequirements : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category="Requirements")
	int32 EssenceQuantity;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category="Requirements")
	int32 CasingQuantity;

	//UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category="Requirements")
};
