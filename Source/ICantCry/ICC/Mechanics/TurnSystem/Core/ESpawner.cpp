#include "ESpawner.h"


// Sets default values
AESpawner::AESpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AESpawner::BeginPlay()
{
	Super::BeginPlay();
	
}


