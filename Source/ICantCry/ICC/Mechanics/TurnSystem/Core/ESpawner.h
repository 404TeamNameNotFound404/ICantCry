#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ESpawner.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AESpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AESpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};
