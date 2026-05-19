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
	void RejoinQueue(const TArray<AMob*>& Emotions, AICC_Player* Player);
	void AssignFirstTurn();
	void AssignFirstTurnByPriority();
	
	AMob* GetMobInQueue() const;
	AMob* GetMobInQueue(AMob* Emotion) const;

	bool CantBuffOthers();
	/**
	 * Look if there's only one mob left battling
	 * @return true if there's only one emotion
	 */
	bool LastMobStanding();
	
	bool AreAnxietyOrCalmAlone();
};
