// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Actors/Bullet/BulletData.h"
#include "../Actors/Bullet/Essence.h"
#include "../Actors/Bullet/Bullet.h"
#include "Recipe.generated.h"


UENUM(BlueprintType)
enum class ERecipeType : uint8
{
    Base  UMETA(DisplayName = "Base Blueprint"),
    Gold  UMETA(DisplayName = "Gold Blueprint")
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERecipeType RequiredBlueprintType;  // Base o Gold
 
	// Tipo di casing richiesto (base/gold)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECasingType RequiredCasingType;
 
	// Quanti casing servono
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredCasingQuantity;

	// Essenze richieste (1 per base, 2 per gold)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEssence> RequiredEssences;
 
	// Proiettile che si ottiene
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBullet ResultBullet;


	FORCEINLINE bool operator==(const FRecipe& Other) const
	{
    	return RequiredBlueprintType == Other.RequiredBlueprintType &&
           	RequiredCasingType == Other.RequiredCasingType &&
           	RequiredCasingQuantity == Other.RequiredCasingQuantity &&
           	ResultBullet == Other.ResultBullet;
	}	
 
	FRecipe()
    : RequiredCasingQuantity(1)
	{}

};
