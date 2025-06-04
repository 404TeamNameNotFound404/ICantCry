// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmptyCasing.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct ICANTCRY_API FEmptyCasing
{
	
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    bool operator==(const FEmptyCasing& Other) const
    {
        return Name == Other.Name;
    }
	
};
