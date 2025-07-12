#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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

	void RecreatePlayer(UWorld* World,FVector& PreviousPosition, FRotator& PreviousRotation, float& CurrentHp, float& CurrentAp) const;
	void StoreBeginPlayerTransform(const FVector& BeginPosition, const FRotator& BeginOrientation) const;
	void SavePlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const;
	
	/**
	 * Load the previous position before loading something else
	 */
	UFUNCTION(BlueprintCallable)
	void LoadLastPlayerTransform();

	UPlayerStats* GetPlayerStats() const;
	UPersistentData* GetPersistentData() const;
	FDamage& GetCurrentDamageData();
	void SetPlayerStats(UPlayerStats* Stats);
	void SetPersistentPlayer(AICC_Player* Player);
	AICC_Player* GetCurrentPlayer() const;
	void SetDamageData(const FDamage& Damage);

	void SetInventory(const FInventory& Inv);
	FInventory& GetInventory();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPersistentData* PersistentData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPlayerStats* PlayerStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FDamage StoredDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FInventory Inventory;
	
	UWorld* TryGetWorld() const;

	UPROPERTY()
	AICC_Player* PersistentPlayer = nullptr;

	UPROPERTY()
	AICC_Player* CurrentPlayer = nullptr;
	
};
