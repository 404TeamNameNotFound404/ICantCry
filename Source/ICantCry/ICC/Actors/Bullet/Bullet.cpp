// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"


UBulletData* FBullet::GetBulletData() const
{
	return BulletData; 
}

void FBullet::SetBulletData(UBulletData* NewData)
{
	BulletData = NewData; 
}

void FBullet::SetQuantity(const int32& NewQuantity)
{
	Quantity = NewQuantity;
}

int32 FBullet::GetQuantity() const
{
	return Quantity;
}

FString FBullet::GetStrongAgainstName() const
{
	FString Result;

	for (const auto& Strong : BulletData->StrongAgainst)
	{
		switch (Strong)
		{
		case Indifference:
			Result += "Indifference, ";
			break;
		case AngerEV:
			Result += "Anger, ";
			break;
		case AngerDv:
			Result += "Anger, ";
			break;
		case FearEV:
			Result += "FearEv, ";
			break;
		case FearDv:
			Result += "FearDv, ";
			break;
		case Disgust:
			Result += "Disgust, ";
			break;
		case Sadness:
			Result += "Sadness, ";
			break;
		case JoyEv:
			Result += "JoyEv, ";
			break;
		case JoyDv:
			Result += "JoyDv, ";
			break;
		case Anxiety:
			Result += "Anxiety, ";
			break;
		case CalmEV:
			Result += "CalmEv, ";
			break;
		case CalmDv:
			Result += "CalmDv, ";
			break;
		case JealousyEV:
			Result += "JealousyEv, ";
			break;
		case JealousyDv:
			Result += "JealousyDv, ";
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

FString FBullet::GetWeakAgainstName() const
{
	FString Result;

	for (const auto& Weak : BulletData->WeakAgainst)
	{
		switch (Weak)
		{
		case Indifference:
			Result += "Indifference, ";
			break;
		case AngerEV:
			Result += "AngerEV, ";
			break;
		case AngerDv:
			Result += "AngerDV, ";
			break;
		case FearEV:
			Result += "FearEV, ";
			break;
		case FearDv:
			Result += "FearDv, ";
			break;
		case Disgust:
			Result += "Disgust, ";
			break;
		case Sadness:
			Result += "Sadness, ";
			break;
		case JoyEv:
			Result += "JoyEv, ";
			break;
		case JoyDv:
			Result += "JoyDv, ";
			break;
		case Anxiety:
			Result += "Anxiety, ";
			break;
		case CalmDv:
			Result += "CalmDv, ";
			break;
		case CalmEV:
			Result += "CalmEV, ";
			break;
		case JealousyDv:
			Result += "Jealousy, ";
			break;
		case JealousyEV:
			Result += "JealousyEv, ";
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


