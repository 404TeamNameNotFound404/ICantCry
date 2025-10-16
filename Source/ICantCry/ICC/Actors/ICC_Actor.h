// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ICantCry/ICC/Actors/StatusTracker/StatusTracker.h"
#include "ICC_Actor.generated.h"

UCLASS()
class ICANTCRY_API AICC_Actor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AICC_Actor();

	/**
	 * Get the Status tracker , a component that handles the status infliction and malus
	 * @return Current Owner Tracker
	 */
	UStatusTracker* GetStatusTracker() const;

	void Freeze(const bool& Value);
	void Burn(const bool& Value);
	void Ashamed(const bool& Value);
	void ShieldDebuff(const bool& Value);

	bool IsFreezed() const;
	bool IsBurned() const;
	bool IsShieldedDebuff() const;
	bool IsAshamed() const;

	virtual int GetSpeed() const;

protected:
	UPROPERTY()
	UStatusTracker* StatusTracker;


private:
	UPROPERTY()
	bool bFreezed = false;

	UPROPERTY()
	bool bBurned = false;

	UPROPERTY()
	bool bAshamed = false;

	UPROPERTY()
	bool bShieldDebuff = false;
};
