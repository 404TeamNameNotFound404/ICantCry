#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RewardsData.h"
#include "PersistentData.generated.h"

/**
 * Data container used to store player's data across levels in order to recreate player during scene
 * transitions.
 * @note All this class within "Data" folder are READ-ONLY!
 */
UCLASS(Blueprintable)
class ICANTCRY_API UPersistentData : public UDataAsset
{
	GENERATED_BODY()

public:
	// First of hall I think it can be good to store Position, Rotation and data such as AP, HP
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	FVector PlayerPosition;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	FRotator PlayerOrientation;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	float PlayerHp;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	float InitialAttackPower;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Player Data")
	float InitialDefencePower;

	// Storing the Rewards Data Container independently to be as clean as possible 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Rewards")
	URewardsData* Rewards;
};
