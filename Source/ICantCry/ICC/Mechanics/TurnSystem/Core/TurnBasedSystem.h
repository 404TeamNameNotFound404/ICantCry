#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Turn/Turn.h"
#include "../../../Managers/EnemySpawnManager.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/Victory/VictoryVisualizer.h"
#include "TurnBasedSystem.generated.h"

class AICC_Player;
class ABattleHandler;

/**
 * Manage and handle The combat
 */
UCLASS()
class ICANTCRY_API UTurnBasedSystem : public UObject
{
	GENERATED_BODY()

public:
	UTurnBasedSystem();
	
	void Start(UWorld* World);
	void Update(UWorld* World);
	void StartNextTurn();
	void EndTurn();
	FTurn GetTurn() const;

	bool GetIsPlayerTurn() const;
	bool GetIsAITurn() const;
	AICC_Player* TryGetCurrentPlayer() const;

	void SetTurnOverlayApplied(const bool &Applied);
	void RequestFight(const bool &Request);
	void SetAIPlaying(const bool &Play);

	TArray<AICC_Actor*> GetCopyQueue() const;

	void SpawnBattleVictory(UWorld* World);

	/**
	 * --------- WIP FUNCTIONS -----------
	 */

	void ExitBattle();
	
	/**
     * --------- WIP FUNCTIONS -----------
     */
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	FTurn Turn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	float Variations = 2.0f;

	/**How much time AI has to play its turn*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	float MaxAITurnTime;
	
	UPROPERTY()
	bool bIsAiTurn;

	UPROPERTY()
	bool bIsPlayerTurn;

	UPROPERTY()
	AICC_Player* CurrentPlayer;

	/**
	 * Check win condition or player death condition at runtime
	 */
	void Flow();

	UPROPERTY()
	AEnemySpawnManager* EnemySpawnManager;

	UPROPERTY()
	ABattleHandler* BattleHandler;

	UPROPERTY()
	bool bTurnOverlayApplied = false;

	UPROPERTY()
	bool bRequestFight = false;

	UPROPERTY()
	bool bFightStarted = false;

	UPROPERTY()
	bool bAIPlayTurn = false;

	UPROPERTY()
	bool bInit = false;

	UPROPERTY()
	bool bVictory = false;

	UPROPERTY()
	TArray<AICC_Actor*> CopyQueue;

	UPROPERTY()
	UVictoryVisualizer* VictoryVisualizer;
};
