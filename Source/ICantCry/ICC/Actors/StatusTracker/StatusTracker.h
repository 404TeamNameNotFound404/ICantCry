// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusTracker.generated.h"

class AICC_Actor;

UENUM()
enum EAfflictedStatus
{
	Freezed,
	Burn,
	EAShame,
	ShieldDebuff,
	None
};

UENUM()
enum EBuffStatus
{
	AtkBuff,
	DefBuff,
	LowHealth
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UStatusTracker : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusTracker();

protected:
	/**
	 * Used to store if the owner of this component is afflicted by
	 * status
	 */
	UPROPERTY()
	bool bIsOwnerAfflicted;

	UPROPERTY()
	bool bIsOwnerAlreadyBuffed;

	UPROPERTY()
	TEnumAsByte<EAfflictedStatus> CurrentActiveStatus;

	UPROPERTY()
	TEnumAsByte<EBuffStatus> CurrentBuffedStatus;

	UPROPERTY()
	int32 TurnElapsed = 0;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	/*-------------------- AI CHECKS --------------------*/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/
	
	/**
	 * Check if there's status applied to the current actor and if so return true.
	 * To be defined
	 * @return true if status is applied
	 */
	bool IsAfflicted() const;

	/**
	 * Check if the owner is buffed
	 * @return true if owner has a buff
	 */
	bool IsBuffed() const;

	/**
	 * Assign Status to afflict
	 * - For AI inside the dedicated behavior
	 * - For Player inside the bullet
	 * @param Status Desired Status to afflict
	 */
	void InflictStatus(const EAfflictedStatus& Status, AICC_Actor* Target);

	/**
	 * Assing the buff to give to target
	 * @param BuffStatus Buff
	 */
	void BuffWith(const EBuffStatus& BuffStatus);

	
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/
	/*-------------------- AI CHECKS --------------------*/

	void UpdateStatus();

	void UpdateBuffStatus();

	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/

	void UnfreezeChance();

	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/


private:

	UPROPERTY()
	int32 StatusCounter = 0;

	UPROPERTY()
	int32 BuffStatusCounter = 0;

	UPROPERTY()
	bool bCanBuff = true;

	void InflictFreeze(AICC_Actor* Target);
	void InflictBurn(AICC_Actor* Target);
	void InflictShieldDebuff(AICC_Actor* Target);
	void InflictAShamed(AICC_Actor* Target);
	void BuffAttack();
	void BuffDefence();
	/**
	 * Used for joy ev / ai 
	 */
	void Heal();
};
