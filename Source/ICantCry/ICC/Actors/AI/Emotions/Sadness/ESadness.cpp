#include "ESadness.h"

AESadness::AESadness()
{
	bIsESadness = true;
	bIsHealer = false;
	bEAnger = false;
	Type = EMobType::MobSadness;
}

// Called when the game starts or when spawned
void AESadness::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AESadness::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


