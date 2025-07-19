#pragma once
#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "Turn.generated.h"

USTRUCT(BlueprintType)
struct FTurn
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turn System", meta = (AllowPrivateAccess = "true"))
	float Timer;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turn System", meta = (AllowPrivateAccess = "true"))
	int32 CurrentTurn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turn System", meta = (AllowPrivateAccess = "true"))
	int32 NextTurn;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turn System", meta = (AllowPrivateAccess = "true"))
	TArray<AICC_Actor*> Queue;
	

	FTurn();
	void PopulateQueue(UWorld* World);
	void AssignFirstTurn();
	
	AMob* GetMobInQueue() const;
	
	/**
* Array that keeps in track how many emotions are in the game
* This array is only meant to be used in order to prevent
* emotions from buff other emotion if they're alone
*/
	TArray<AMob*> GetEmotionsInBattle() const;

	bool CantBuffOthers();

private:

	UPROPERTY()
	TArray<AMob*> EmotionsInBattle;
};
