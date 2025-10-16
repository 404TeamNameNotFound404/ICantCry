// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ICantCry/ICC/Actors/AI/MobType.h"
#include "BulletData.generated.h"


UENUM(BlueprintType)
enum EBulletStatus
{
    FreezedUp,
    EnvyBurned,
    Ashamed,
    DebuffShield
};

UENUM(BlueprintType)
enum EBulletType
{
	Indifference,
	AngerDv,
	AngerEv,
	FearDv,
	FearEV,
	Disgust,
	SadnessDv,
	JoyDv,
	JoyEv,
	Anxiety,
	CalmDv,
	CalmEV,
	JealousyDv,
	JealousyEv,
	SadnessEv,
	Shame
};

UENUM(BlueprintType)
enum EEmotionType
{
	Emotion_Anger,
	Emotion_Shame,
	Emotion_Joy,
	Emotion_Disgust,
	Emotion_Fear,
	Emotion_Jealousy,
	Emotion_Sadness,
	Emotion_Anxiety,
	Emotion_Calm
};


/**
 * 
 */
UCLASS()
class ICANTCRY_API UBulletData : public UDataAsset
{
	GENERATED_BODY()
	

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FName ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    int32 Power;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString BulletName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString MinigameType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	FString Description;
	
	// for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	TArray<TEnumAsByte<EEmotionType>> WeakAgainst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	TArray<TEnumAsByte<EEmotionType>> StrongAgainst;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	FColor DisplayColor;

	/**
	 * Determines the strength of the bullet based on its value.
	 * Example: A Coefficient of 0.5 makes the bullet less effective
	 *  while a value of 1.5 makes it super effective.
	 */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Properties")
	// float Coefficient = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Properties")
	TMap<TEnumAsByte<EMobType>, float> Coefficients;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	TEnumAsByte<EBulletType> Type;
	
	/**
 * Weakness modifier
 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float WeaknessModifier = 0.0f;

};
