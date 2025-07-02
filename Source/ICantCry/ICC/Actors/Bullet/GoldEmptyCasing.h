// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Casing.h"
#include "GoldEmptyCasing.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct ICANTCRY_API FGoldEmptyCasing : public FCasing
{
    GENERATED_BODY()

public:
    
    bool operator==(const FGoldEmptyCasing& Other) const
    {
        return Name == Other.Name;
    }
};
