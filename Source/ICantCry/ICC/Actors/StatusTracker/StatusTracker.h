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
	DebuffAtk,
	DebuffDef,
	None
};

UENUM()
enum EBuffStatus
{
	AtkBuff,
	DefBuff,
	LowHealth,
	Shield,
	NoBuff
};


USTRUCT()
struct FInternalPerkData
{
	GENERATED_BODY();

	UPROPERTY() bool bBuffAtk;
	UPROPERTY() bool bBuffDef;
	UPROPERTY() bool bLowHealth;
	UPROPERTY() bool bFreezedUp;
	UPROPERTY() bool bAshamed;
	UPROPERTY() bool bShieldDebuff;
	UPROPERTY() bool bDebuffAtk;
	UPROPERTY() bool bDebuffDef;
	UPROPERTY() bool bEnvyBurned;

	void Clear()
	{
		bBuffAtk = false;
		bBuffDef = false;
		bLowHealth = false;
		bFreezedUp = false;
		bShieldDebuff = false;
		bDebuffAtk = false;
		bDebuffDef = false;
		bAshamed = false;
		bEnvyBurned = false;
	}
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

	/**
 * Update the malus status counting 3 turns starting from the turn Player / AI
 * activated the buff
 */
	void UpdateStatus();

	/**
	 * Update the buff status counting 3 turns starting from the turn Player / AI
	 * activated the buff
	 */
	void UpdateBuffStatus();

	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/

	/**
	 * Attempt to unfreeze each player turn (25% chance)
	 */
	void UnfreezeChance();
	
	/**
* Handles the status rules:
*  check if Another buff is applied and the AI is buffed the current buff is replaced with the new one
*/
	void BuffFlow(const EBuffStatus& NewBuffStatus);


	/**
	 * Check If AI / Player is buffed and is being target by a debuff the current buff removed and the debuff is not applied
	 */
	void MalusFlow();

	FInternalPerkData& GetPerkData();

	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/

	FString GetStatusName(const EAfflictedStatus& Status) const;
	FString GetBuffName(const EBuffStatus& Buff) const;

private:

	UPROPERTY()
	int32 StatusCounter = 0;

	UPROPERTY()
	int32 BuffStatusCounter = 0;

	UPROPERTY()
	bool bCanBuff = true;

	UPROPERTY()
	bool bCanDebuff = true;

	UPROPERTY()
	FInternalPerkData PerkData;

	void InflictFreeze(AICC_Actor* Target);
	void InflictBurn(AICC_Actor* Target);
	void InflictShieldDebuff(AICC_Actor* Target);
	void InflictAShamed(AICC_Actor* Target);
	void BuffAttack();
	void BuffDefence();
	void BuffShield();
	
	/**
	 * Used for joy ev / ai 
	 */
	void Heal();

	void DebuffAtkF();
	void DebuffDefF();

	/**
	 * Rollback the current status state
	 */
	void RevertInflictedMalus(const EAfflictedStatus& Status);

	void RevertBuff();
};
