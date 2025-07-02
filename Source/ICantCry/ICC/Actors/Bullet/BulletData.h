// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	TArray<TEnumAsByte<EBulletType>> WeakAgainst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	TArray<TEnumAsByte<EBulletType>> StrongAgainst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	FColor DisplayColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
    float WeaknessModifier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
    TEnumAsByte<EBulletStatus> Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	TEnumAsByte<EBulletType> Type;
	
	// for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* Icon;

    
};
