// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Essence.generated.h"


UENUM(BlueprintType)
enum class EEssenceType : uint8
{
    Indifference,
    Anger,
    Fear,
    Disgust,
    Sadness,
    Joy,
    Anxiety,
    Calm,
    Jealousy,
    Shame
};



USTRUCT(BlueprintType)
struct ICANTCRY_API FEssence
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EssenceName;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEssenceType EssenceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    bool IsValid() const
    {
        return !EssenceName.IsEmpty() && Quantity >= 0;
    }

    FORCEINLINE bool operator==(const FEssence& Other) const
    {
        return EssenceType == Other.EssenceType;
    }

    FString GetName(const EEssenceType& Type) const
    {
        switch (Type)
        {
            case EEssenceType::Indifference:
                return "Indifference";
            case EEssenceType::Anger:
                return "Anger";
            case EEssenceType::Fear:
                return "Fear";
            case EEssenceType::Disgust:
                return "Disgust";
            case EEssenceType::Sadness:
                return "Sadness";
            case EEssenceType::Joy:
                return "Joy";
            case EEssenceType::Anxiety:
                return "Anxiety";
            case EEssenceType::Calm:
                return "Calm";
            case EEssenceType::Jealousy:
                return "Jealousy";
            case EEssenceType::Shame:
                return "Shame";
            default:
                return "";
        }
    }

    FEssence()
        : EssenceType(), Quantity()
    {}
};
