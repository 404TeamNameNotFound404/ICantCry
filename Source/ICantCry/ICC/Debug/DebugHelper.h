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
private:
	static TArray<FString> LogMessages;
	
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

	/**
	 * Add Overlay Material to a static mesh only
	 * @param Target the static mesh
	 */
	static void AddOverlayMaterialToStaticMesh(UStaticMeshComponent* Target);

	/**
	 * Add Overlay material to a skeletal mesh component, the overlay path is PREDEFINED so it will take the material name
	 * within /Game/ICC/BluePrints/Actors/Materials/OutLiner/ Folder!
	 * @note /Game/ICC/BluePrints/Actors/Materials/OutLiner/ is the predefined path!
	 * @param Target Skeletal mesh component you want to apply
	 * @param MaterialName Material name
	 */
	static void AddOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target, const FString& MaterialName);

	static void AddTurnOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target);

	static void RemoveTurnOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target);

	/**
	 * Remove the overlay material from a skeletal mesh component
	 * @param Target skeletal mesh
	 */
	static void RemoveOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target);

	/**
	 * Remove the overlay material from a static mesh component
	 * @param Target 
	 */
	static void RemoveOverlayMaterialFromStaticMesh(UStaticMeshComponent* Target);

	/**
	 * Same function as above but it loads the M_TurnOutliner material
	 * @param Target static mesh
	 */
	static void AddTurnMaterialOverlayToStaticMesh(UStaticMeshComponent* Target);

	/**
	 * Same as remove
	 * @param Target static mesh
	 */
	static void RemoveTurnMaterialOverlayToStaticMesh(UStaticMeshComponent* Target);

	/**
	 * Add messages to array
	 * @param Message Message
	 */
	static void AddMessageToLog(const FString& Message);

	/**
	 * Clear every logs
	 */
	static void ClearAllLogs();
	
	/**
	 * Write File to log
	 */
	static void SaveLogToFile();

	static bool IsGamepadPlugged();
	
};
