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
	void RejoinQueue(const TArray<AMob*>& Emotions);
	void AssignFirstTurn();
	
	AMob* GetMobInQueue() const;

	bool CantBuffOthers();
};
