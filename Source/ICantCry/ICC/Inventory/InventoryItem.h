// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Actors/Bullet/Bullet.h"
#include "../Inventory/Recipe.h"
#include "../Actors/Bullet/Essence.h"
#include "../Actors/Bullet/EmptyCasing.h"
#include "../Actors/Bullet/GoldEmptyCasing.h"
#include "InventoryItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Bullet,
    Essence,
    Recipe,
	EmptyCasing,
    GoldEmptyCasing
};


USTRUCT(BlueprintType)
struct ICANTCRY_API FInventoryItem
{

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBullet Bullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FEmptyCasing EmptyCasing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGoldEmptyCasing GoldEmptyCasing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEssence> Essences; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRecipe Recipe;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    FInventoryItem()
        : ItemType(EItemType::Bullet), Quantity(0)
    {}
	
};
