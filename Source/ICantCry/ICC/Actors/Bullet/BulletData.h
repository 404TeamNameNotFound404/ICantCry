// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BulletData.generated.h"

UCLASS()
class ICANTCRY_API UBulletData : public UDataAsset
{
	GENERATED_BODY()
	

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    int32 Power;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString BulletName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
    FString MinigameType;

	// for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* Icon;

	/**
 * Weakness modifier
 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modifiers")
	float WeaknessModifier = 0.0f;
};
