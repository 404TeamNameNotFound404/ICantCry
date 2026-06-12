// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletData.h" 
#include "Bullet.generated.h"


class AICC_Actor;

USTRUCT(Blueprintable)
struct ICANTCRY_API FBullet
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	UBulletData* BulletData;
	
	UPROPERTY()
	int32 Quantity;
	
public:

	FBullet() : BulletData(nullptr), Quantity(0)
	{
	}

	FBullet(UBulletData* InData) : BulletData(InData), Quantity(0)
	{
	}
	
	FORCEINLINE bool operator==(const FBullet& Other) const
	{
    	return BulletData && Other.BulletData && BulletData->ID == Other.BulletData->ID; 
	}
	
	UBulletData* GetBulletData() const;
	
	void SetBulletData(UBulletData* NewData);

	void SetQuantity(const int32& NewQuantity);

	int32 GetQuantity() const;

	FString GetStrongAgainstName() const;

	FString GetWeakAgainstName() const;

	FBullet Clone() const;
	
	bool IsValid() const;
};