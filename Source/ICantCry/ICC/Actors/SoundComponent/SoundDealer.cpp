#include "SoundDealer.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"

USoundDealer::USoundDealer()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USoundDealer::PlayFootStep()
{
	if (!FootStepSfx) return;
	UGameplayStatics::PlaySoundAtLocation(this, FootStepSfx, GetOwner()->GetActorLocation());
}

void USoundDealer::PlayRandomFootSteps()
{
	if (FootStepsSfx.IsEmpty()) return;

	int32 RandomIndex;

	if (FootStepsSfx.Num() > 1)
	{
		do
		{
			RandomIndex = FMath::RandRange(
				0,
				FootStepsSfx.Num() - 1
			);
		}
		while (RandomIndex == LastFootstepIndex);
	}
	else
	{
		RandomIndex = 0;
	}

	LastFootstepIndex = RandomIndex;

	USoundBase* ChosenOne = FootStepsSfx[RandomIndex];

	if (!ChosenOne) return;

	UGameplayStatics::PlaySoundAtLocation(this,ChosenOne,GetOwner()->GetActorLocation());
}

void USoundDealer::FireInteraction()
{
	if (!InteractionSfx) return;
	UGameplayStatics::PlaySound2D(this, InteractionSfx);
}

void USoundDealer::FireDoorInteraction()
{
	if (!DoorInteractionSfx) return;
	UGameplayStatics::PlaySound2D(this, DoorInteractionSfx);
}

void USoundDealer::FireEncounterInteraction()
{
	if (!EncounterInteractionSfx) return;
	UGameplayStatics::PlaySound2D(this, EncounterInteractionSfx);
}


// Called when the game starts
void USoundDealer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


