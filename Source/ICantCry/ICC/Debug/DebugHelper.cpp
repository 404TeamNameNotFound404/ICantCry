// Fill out your copyright notice in the Description page of Project Settings.
#include "DebugHelper.h"
#include "EngineUtils.h"

void DebugHelper::LogSuccess(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Green, Message);
}

void DebugHelper::LogWarning(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Yellow, Message);
}

void DebugHelper::LogError(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, Message);
}

void DebugHelper::LogMessage(const float& TimeToDisplay, const FColor& Color, const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, Message);
}


AActor* DebugHelper::FindActorWithTag(UWorld* World, const FName& Tag)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->ActorHasTag(Tag))
		{
			return *It;
		}
		break;
	}

	return nullptr;
}

AActor* DebugHelper::FindActorByName(UWorld* World, const FName& Name)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetName() == Name)
		{
			return *It;
		}
		
		break;
	}

	return nullptr;
}
