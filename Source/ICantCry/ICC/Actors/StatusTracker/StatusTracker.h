// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
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
	CriticHealth,
	None
};

UENUM()
enum EBuffStatus
{
	AtkBuff,
	DefBuff,
	Shield,
	LowHealth,
	NoBuff
};

UENUM()
enum EPrioritySource
{
	Source_None,
	Source_BuffAtk,
	Source_DebuffAtk,
	Source_BuffDef,
	Source_DebuffDef,
	Source_LowHealth,
	Source_EnvyBurned,
	Source_FreezedUp,
	Source_Ashamed,
	Source_Shield
};

class AMob;

USTRUCT()
struct FStatusPriority
{
	GENERATED_BODY()

	FStatusPriority() = default;
	~FStatusPriority() = default;

	// Status (Malus)
	void SetPriotity(const int32& Value){CurrentPriority = Value;}
	int32 GetPriority() const { return CurrentPriority; }
	int32 GetNextPriority() const { return NextPriority; }

	int32 GetBuffPriority() const {return CurrentBuffPriority;};
	int32 GetNextBuffPriority() const {return NextBuffPriority;};
	void SetBuffCurrentPriority(const int32& Value) { CurrentBuffPriority = Value; }
	
	void SetNextPriorityFromBuff(const EBuffStatus& BuffStatus);
	void SetNextPriorityFromDebuff(const EAfflictedStatus& Status);
	bool CanUsePriority(const EMobType& EmotionType, const EPrioritySource& SourcePriority) const;
	void ClearNextBuff();

	EPrioritySource GetNextPrioritySource() const;
	EPrioritySource GetCurrentPrioritySource() const;
	void CommitNextBuff();

private:
	UPROPERTY()  int32 NormalPriority = 0;
	UPROPERTY()  int32 BuffAtkPriority = 1;
	UPROPERTY()  int32 DeBuffAtkPriority = 1;
	UPROPERTY()  int32 BuffDefPriority = 2;
	UPROPERTY()  int32 DeBuffDefPriority = 2;
	UPROPERTY()  int32 LowHealthPriority = 3;
	UPROPERTY()  int32 EnvyBurnedPriority = 4;
	UPROPERTY()  int32 AshamedPriority = 4;
	UPROPERTY()  int32 DebuffShieldPriority = 4;
	UPROPERTY() TEnumAsByte<EPrioritySource> CurrentBuffSource = EPrioritySource::Source_None;
	UPROPERTY() TEnumAsByte<EPrioritySource> NextBuffSource = EPrioritySource::Source_None;

	// Status (Malus)
	UPROPERTY() int32 CurrentPriority = 0; // current buff/debuff AI has
	UPROPERTY() int32 NextPriority = 0; // the next buff / debuff AI wants to cast

	// Status (Buff)
	UPROPERTY() int32 CurrentBuffPriority = 0;
	UPROPERTY() int32 NextBuffPriority = 0;
	
	void SetNextPriority(const int32& Value) { NextPriority = Value; }
	void SetNextBuffProcessPriority(const int32& Value) { NextBuffPriority = Value; }
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
	UPROPERTY() bool bIdle;

	FStatusPriority Priority;

	void CheckPriority(AMob* Mob);
	
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
		bIdle = false;
	}

	bool HasBuffHighPriority(AMob* Emotion) const;
	bool HasHighDebuffPriority(AMob* Emotion) const;
	void AssignPriority(AMob* Emotion);
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
	TEnumAsByte<EBuffStatus> CurrentBuffedStatus = EBuffStatus::NoBuff;

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

	bool CanDebuff() const;

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
	void BuffFlow(const EBuffStatus& NewBuffStatus, AMob* Target);


	/**
	 * Check If AI / Player is buffed and is being target by a debuff the current buff removed and the debuff is not applied
	 */
	void MalusFlow();

	FInternalPerkData& GetPerkData();

	FStatusPriority& GetStatusPriority();

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

	UPROPERTY()
	int32 PlayerShieldAccumulator = 0;

	UPROPERTY()
	bool bShieldBuffed = false;

	UPROPERTY()
	FStatusPriority Priority;

	UPROPERTY()
	UICantCryGameInstance* Instance;

	UPROPERTY()
	bool bBuffedTwice = false;
	
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

	void ApplyPriorityBuff(const EBuffStatus& BuffStatus, AMob* Target);
};
