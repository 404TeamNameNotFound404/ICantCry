// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletData.h" 
#include "Bullet.generated.h"

USTRUCT(Blueprintable)
struct ICANTCRY_API FBullet
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	UBulletData* BulletData;

public:

	FBullet() : BulletData(nullptr) {}

	FBullet(UBulletData* InData) : BulletData(InData) {}

	// to compare Recipe->ResultBullet == BulletToCraft
	FORCEINLINE bool operator==(const FBullet& Other) const
	{
    	return BulletData && Other.BulletData && BulletData->ID == Other.BulletData->ID; 
	}

	
	UBulletData* GetBulletData() const 
	{ 
		return BulletData; 
	}

	
	void SetBulletData(UBulletData* NewData) 
	{ 
		BulletData = NewData; 
	}

};