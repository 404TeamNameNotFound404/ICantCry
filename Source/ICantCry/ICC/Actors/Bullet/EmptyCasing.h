// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Casing.h"
#include "EmptyCasing.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct ICANTCRY_API FEmptyCasing : public FCasing
{
	
	GENERATED_BODY()

    bool operator==(const FEmptyCasing& Other) const
    {
        return Name == Other.Name;
    }
	
};
