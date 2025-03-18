#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Turn/Turn.h"
#include "TurnBasedSystem.generated.h"

/**
 * Manage and handle The combat
 */
UCLASS()
class ICANTCRY_API UTurnBasedSystem : public UObject
{
	GENERATED_BODY()

public:
	UTurnBasedSystem();
	
	void Start();
	void Update();
	void End();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	FTurn Turn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	float Variations;

	/**How much time AI has to play its turn*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	float MaxAITurnTime;
	
	UPROPERTY()
	bool bIsAiTurn;

	UPROPERTY()
	bool bIsPlayerTurn;
	
	void AssignFirstTurn();
};
