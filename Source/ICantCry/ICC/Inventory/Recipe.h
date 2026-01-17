// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Actors/Bullet/BulletData.h"
#include "../Actors/Bullet/Essence.h"
#include "../Actors/Bullet/Bullet.h"
#include "ICantCry/ICC/Mechanics/Core/Data/BPRequirements.h"
#include "Recipe.generated.h"


UENUM(BlueprintType)
enum class ERecipeType : uint8
{
	Base UMETA(DisplayName = "Base Blueprint"),
	Gold UMETA(DisplayName = "Gold Blueprint"),
	Anger,
	AngerEv,
	Sadness,
	Anxiety,
	Shame,
	Disgust,
	Joy,
	JoyEv,
	FearEv,
	Fear,
	Calm,
	CalmEv,
	Jealousy,
	JealousyEv,
	Indifference
};

UENUM(BlueprintType)
enum class ECasingType : uint8
{
	Base UMETA(DisplayName = "Base Casing"),
	Gold UMETA(DisplayName = "Gold Casing")
};


USTRUCT(BlueprintType)
struct ICANTCRY_API FRecipe
{
	GENERATED_BODY()

public:
	// Tipo blueprint richiesto
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ERecipeType RequiredBlueprintType; // Base o Gold

	// Tipo di casing richiesto (base/gold)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECasingType RequiredCasingType;

	// Quanti casing servono
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredCasingQuantity;

	// Essenze richieste (1 per base, 2 per gold)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEssence> RequiredEssences;

	// Proiettile che si ottiene
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBullet ResultBullet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBPRequirements* Requirements;

	/**
	 * Blueprint Description
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Index;
	
	FString GetName(const ERecipeType& RecipeType) const;

	FString GetCaseType(const ECasingType& Type) const;

	FString GetEssencesName() const;

	FText DisplayDescription() const
	{
		return Description;
	}

	FORCEINLINE bool operator==(const FRecipe& Other) const
	{
		return RequiredBlueprintType == Other.RequiredBlueprintType &&
			RequiredCasingType == Other.RequiredCasingType &&
			RequiredCasingQuantity == Other.RequiredCasingQuantity &&
			ResultBullet == Other.ResultBullet;
	}

	FRecipe()
		: RequiredBlueprintType(), RequiredCasingType(), RequiredCasingQuantity(1)
	{
	}
	
	bool IsValid() const
	{
		return RequiredEssences.Num() > 0;
	}



};
