// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"


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

	/**
	 * Find An Actor using a tag
	 * @param World Current World
	 * @param Tag Tag
	 * @return Selected Tag Actor
	 */
	static AActor* FindActorWithTag(UWorld* World, const FName& Tag);
	
	/**
	 * Attempt to find Actor by name
	 * @param World Current
	 * @param Name Name
	 * @return Desired Actor if found
	 */
	static AActor* FindActorByName(UWorld* World, const FName& Name);

	static void AddOverlayMaterialToStaticMesh(UStaticMeshComponent* Target);

	static void RemoveOverlayMaterialFromStaticMesh(UStaticMeshComponent* Target);
};
