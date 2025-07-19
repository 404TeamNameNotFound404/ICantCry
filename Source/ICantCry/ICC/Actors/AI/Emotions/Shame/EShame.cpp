#include "EShame.h"


// Sets default values
AEShame::AEShame()
{
	bIsEShame = true;
	Type = EMobType::MobShame;
}

// Called when the game starts or when spawned
void AEShame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEShame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

