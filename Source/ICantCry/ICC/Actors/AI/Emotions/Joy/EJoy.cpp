#include "EJoy.h"

AEJoy::AEJoy()
{
	bIsHealer = true;
	Type = EMobType::MobJoy;
}


void AEJoy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEJoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEJoy::Heal(const float& RestoredHealth)
{
	Super::Heal(RestoredHealth);
}

