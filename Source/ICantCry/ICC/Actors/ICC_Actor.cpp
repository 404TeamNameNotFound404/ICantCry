// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_Actor.h"

#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "Player/ICC_Player.h"


// Sets default values
AICC_Actor::AICC_Actor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	StatusTracker = CreateDefaultSubobject<UStatusTracker>("StatusTracker");
}

UStatusTracker* AICC_Actor::GetStatusTracker() const
{
	return StatusTracker;
}

void AICC_Actor::Freeze(const bool& Value)
{
	bFreezed = Value;
}

void AICC_Actor::Burn(const bool& Value)
{
	bBurned = Value;
}

void AICC_Actor::Ashamed(const bool& Value)
{
	bAshamed = Value;
}

void AICC_Actor::ShieldDebuff(const bool& Value)
{
	bShieldDebuff = Value;
}


bool AICC_Actor::IsFreezed() const
{
	return bFreezed;
}

bool AICC_Actor::IsBurned() const
{
	return bBurned;
}

bool AICC_Actor::IsShieldedDebuff() const
{
	return bShieldDebuff;
}

bool AICC_Actor::IsAshamed() const
{
	return bAshamed;
}

int AICC_Actor::GetSpeed() const
{
	return -1;
}

void AICC_Actor::FreezeInput()
{
	if (AICC_Player* Player = Cast<AICC_Player>(this))
	{
		AICC_PlayerController* Controller2 = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
		if (!Controller) return;
		DisableInput(Controller2);
		Player->GetMovementComponent()->Velocity = FVector::Zero();
		DebugHelper::LogMessage(4, FColor::White, "Controller disabled");
	}
}

void AICC_Actor::UnfreezeInput()
{
	if (Cast<AICC_Player>(this))
	{
		AICC_PlayerController* Controller2 = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
		if (!Controller) return;
		EnableInput(Controller2);
		DebugHelper::LogMessage(4, FColor::White, "Controller re-enabled");
	}
}

FLinearColor AICC_Actor::GetTurnOverlayColor() const
{
	return TurnOutlinerColor;
}


