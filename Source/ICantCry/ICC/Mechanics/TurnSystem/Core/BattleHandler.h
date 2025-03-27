#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnBasedSystem.h"
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

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTurnBasedSystem* TurnBasedSystem;
};
