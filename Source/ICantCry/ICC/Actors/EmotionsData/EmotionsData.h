// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmotionsData.generated.h"

UENUM(BlueprintType)
enum class EEmotionType : uint8
{
	Default   UMETA(DisplayName=" "),
	Anger     UMETA(DisplayName="Anger"),
    Fear      UMETA(DisplayName="Fear"),
    Disgust   UMETA(DisplayName="Disgust"),
    Sadness   UMETA(DisplayName="Sadness"),
    Joy       UMETA(DisplayName="Joy"),
    Anxiety   UMETA(DisplayName="Anxiety"),
    Calm      UMETA(DisplayName="Calm"),
    Jealousy  UMETA(DisplayName="Jealousy"),
    Shame     UMETA(DisplayName="Shame")
};

USTRUCT(BlueprintType)
struct FEmotionStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EEmotionType> Weaknesses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EEmotionType> Resistances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Moves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NarrativeInformation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> BattleBarks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* EmotionTexture; 
};



UCLASS()
class ICANTCRY_API UEmotionsData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EEmotionType, FEmotionStats> EmotionStats;
};
