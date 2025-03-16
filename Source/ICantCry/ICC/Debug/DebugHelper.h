// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Service class to debug on screen
 */
class ICANTCRY_API DebugHelper
{
public:
	static void LogSuccess(const FString& Message);
	static void LogWarning(const FString& Message);
	static void LogError(const FString& Message);
	static void LogMessage(const float& TimeToDisplay, const FColor& Color, const FString& Message);
};
