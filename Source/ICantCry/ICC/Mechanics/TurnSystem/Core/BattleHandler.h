#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Mechanics/UI/BattleNotifiers/BattleInfo.h"
#include "ICantCry/ICC/Managers/EnemySpawnManager.h"
#include "BattleHandler.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API ABattleHandler : public AActor
{
	GENERATED_BODY()

public:
	ABattleHandler();


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UTurnBasedSystem* GetTurnBasedSystem() const;

	UBattleInfo* GetBattleInfo() const;

	AEnemySpawnManager* GetEnemySpawnManager();

	bool IsControllerPlugged() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTurnBasedSystem* TurnBasedSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Battle Info" ,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UBattleInfo> BattleInfoWidget ;

	UPROPERTY()
	bool bControllerPlugged = false;

	UPROPERTY()
	UBattleInfo* BattleInfo;

	UPROPERTY()
	AEnemySpawnManager* SpawnManager;

	UPROPERTY()
	AICC_PlayerController* PC;
};
