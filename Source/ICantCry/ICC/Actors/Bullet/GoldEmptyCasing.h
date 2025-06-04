// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoldEmptyCasing.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct ICANTCRY_API FGoldEmptyCasing
{

    GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;
    
    bool operator==(const FGoldEmptyCasing& Other) const
    {
        return Name == Other.Name;
    }

	
};
