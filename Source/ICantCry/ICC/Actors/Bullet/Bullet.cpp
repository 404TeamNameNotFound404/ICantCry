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
		case Emotion_Anger:
			Result  += " Anger";
			break;
		case Emotion_Shame:
			Result  += " Shame";
			break;
		case Emotion_Joy:
			Result  += " Joy";
			break;
		case Emotion_Disgust:
			Result  += " Disgust";
			break;
		case Emotion_Fear:
			Result  += " Fear";
			break;
		case Emotion_Jealousy:
			Result  += " Jealousy";
			break;
		case Emotion_Sadness:
			Result  += " Sadness";
			break;
		case Emotion_Anxiety:
			Result  += " Anxiety";
			break;
		case Emotion_Calm:
			Result  += " Calm";
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

	for (const auto& Strong : BulletData->StrongAgainst)
	{
		switch (Strong)
		{
		case Emotion_Anger:
			Result  += " Anger";
			break;
		case Emotion_Shame:
			Result  += " Shame";
			break;
		case Emotion_Joy:
			Result  += " Joy";
			break;
		case Emotion_Disgust:
			Result  += " Disgust";
			break;
		case Emotion_Fear:
			Result  += " Fear";
			break;
		case Emotion_Jealousy:
			Result  += " Jealousy";
			break;
		case Emotion_Sadness:
			Result  += " Sadness";
			break;
		case Emotion_Anxiety:
			Result  += " Anxiety";
			break;
		case Emotion_Calm:
			Result  += " Calm";
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
