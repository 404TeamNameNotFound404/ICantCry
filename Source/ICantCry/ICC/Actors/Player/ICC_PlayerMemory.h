#pragma once

#include "CoreMinimal.h"
#include "ICC_PlayerMemory.generated.h"

USTRUCT()
struct FPlayerMemory
{
	GENERATED_BODY()
public:
	/**
	 * Store the current location of the player
	 */
	FVector CurrentLocation;
	/**
	 * Store the current orientation (rotation) of the player
	 */
	FRotator CurrentOrientation;
	
	FVector InitialPosition;
	FRotator InitialOrientation;

	FVector LastPositionBeforeBattle;
	FRotator LastOrientationBeforeBattle;
	
	UPROPERTY() bool bIsWorldCameraEnabled;
	UPROPERTY() FVector LastWorldCameraPosition;
	UPROPERTY() FRotator LastWorldCameraRotation;
	
	FPlayerMemory();
};


USTRUCT()
struct FCameraMemory
{
	GENERATED_BODY()
public:
	FRotator SavedControlRotation;
	FCameraMemory();
};
