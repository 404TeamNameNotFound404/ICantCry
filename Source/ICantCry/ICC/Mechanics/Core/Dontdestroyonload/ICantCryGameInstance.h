#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ICantCry/ICC/Actors/Player/ICC_PlayerMemory.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PersistentData.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "ICantCry/ICC/Inventory/Inventory.h"
#include "ICantCryGameInstance.generated.h"

class AICC_Player;

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

	void RecreatePlayer() const;
	/**
	 * Function is deprecated please use StoreBeginPlayerTransform
	 * @param BeginPosition 
	 * @param BeginOrientation 
	 */
	[[deprecated]] void StoreBeginPlayerTransform(const FVector& BeginPosition, const FRotator& BeginOrientation) const;
	void StoreBeginPlayerTransform(AICC_Player* Player, const FVector BeginPosition, const FRotator& BeginOrientation);
	void StoreLastPlayerTransform(AICC_Player* Player, const FVector& LastPosition, const FRotator& LastOrientation);
	void StoreLastPlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const;
	void SavePlayerTransformBegin(AICC_Player* Player, const bool& PreFight = false);
	[[deprecated]] void SavePlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const;

	UPlayerStats* GetPlayerStats() const;
	UPersistentData* GetPersistentData() const;
	FDamage& GetCurrentDamageData();
	void SetPlayerStats(UPlayerStats* Stats);
	void SetPersistentPlayer(AICC_Player* Player);
	AICC_Player* GetCurrentPlayer() const;
	void SetDamageData(const FDamage& Damage);

	void SetInventory(const FInventory& Inv);
	FInventory& GetInventory();

	bool CanRecreatePlayer() const;
	void SetCanRecreatePlayer(const bool& Value);
	FPlayerMemory& GetPlayerRuntimeData();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPersistentData* PersistentData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPlayerStats* PlayerStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FDamage StoredDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FInventory Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	TSubclassOf<AICC_Player> PlayerBp;
	
	UWorld* TryGetWorld() const;

	UPROPERTY()
	AICC_Player* PersistentPlayer = nullptr;

	UPROPERTY()
	AICC_Player* CurrentPlayer = nullptr;

	UPROPERTY()
	FPlayerMemory PlayerRuntimeData;

	UPROPERTY()
	bool bCanRecreatePlayer = false;
	
};
