#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Turn/Turn.h"
#include "../../../Managers/EnemySpawnManager.h"
#include "TurnBasedSystem.generated.h"

class AICC_Player;
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
	
};
