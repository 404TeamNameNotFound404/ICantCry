#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ICantCryGameInstance.generated.h"

/**
 * DontDestroyOnLoad like class
 * @note class should be added in project settings -> instance -> add this 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UICantCryGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void RecreatePlayer(UWorld* World,FVector& PreviousPosition, FRotator& PreviousRotation, float& CurrentHp, float& CurrentAp) const;
	void StorePlayerData(const FVector& BeginPosition, const FRotator& BeginOrientation);
	
private:
	// First of hall I think it can be good to store Position, Rotation and data such as AP, HP
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="DontDestroyOnLoad",meta=(allowPrivateAccess=true))
	FVector PlayerPosition;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="DontDestroyOnLoad",meta=(allowPrivateAccess=true))
	FRotator PlayerOrientation;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="DontDestroyOnLoad",meta=(allowPrivateAccess=true))
	float PlayerHp;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="DontDestroyOnLoad",meta=(allowPrivateAccess=true))
	float AbilityPoints;

	// Reward sections to keep them 
	
	UWorld* TryGetWorld() const;
	
};
