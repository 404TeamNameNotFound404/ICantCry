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
	
	UBulletData* GetBulletData() const 
	{ 
		return BulletData; 
	}

	
	void SetBulletData(UBulletData* NewData) 
	{ 
		BulletData = NewData; 
	}

	void SetQuantity(const int32& NewQuantity)
	{
		Quantity = NewQuantity;
	}

	int32 GetQuantity() const
	{
		return Quantity;
	}

	FString GetStrongAgainstName() const
	{
		FString Result;

		for (const auto& Strong : BulletData->StrongAgainst)
		{
			switch (Strong)
			{
			case Indifference:
				Result += "Indifference, ";
				break;
			case Anger:
				Result += "Anger, ";
				break;
			case Fear:
				Result += "Fear, ";
				break;
			case Disgust:
				Result += "Disgust, ";
				break;
			case Sadness:
				Result += "Sadness, ";
				break;
			case Joy:
				Result += "Joy, ";
				break;
			case Anxiety:
				Result += "Anxiety, ";
				break;
			case Calm:
				Result += "Calm, ";
				break;
			case Jealousy:
				Result += "Jealousy, ";
				break;
			case Shame:
				Result += "Shame, ";
				break;
			default:
				break;
			}
		}

	
		if (Result.EndsWith(", "))
		{
			Result.LeftChopInline(2);
		}

		return Result;
	}

	FString GetWeakAgainstName() const
	{
		FString Result;

		for (const auto& Weak : BulletData->WeakAgainst)
		{
			switch (Weak)
			{
			case Indifference:
				Result += "Indifference, ";
				break;
			case Anger:
				Result += "Anger, ";
				break;
			case Fear:
				Result += "Fear, ";
				break;
			case Disgust:
				Result += "Disgust, ";
				break;
			case Sadness:
				Result += "Sadness, ";
				break;
			case Joy:
				Result += "Joy, ";
				break;
			case Anxiety:
				Result += "Anxiety, ";
				break;
			case Calm:
				Result += "Calm, ";
				break;
			case Jealousy:
				Result += "Jealousy, ";
				break;
			case Shame:
				Result += "Shame, ";
				break;
			default:
				break;
			}
		}
		
		if (Result.EndsWith(", "))
		{
			Result.LeftChopInline(2);
		}

		return Result;
	}


};