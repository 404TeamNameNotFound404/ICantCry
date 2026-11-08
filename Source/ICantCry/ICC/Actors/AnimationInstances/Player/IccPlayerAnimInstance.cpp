// Fill out your copyright notice in the Description page of Project Settings.
#include "IccPlayerAnimInstance.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UIccPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!OwningPlayer)
	{
		OwningPlayer = Cast<AICC_Player>(TryGetPawnOwner());
	}

	if (!OwningPlayer)
	{
		return;
	}
	
	const FVector Velocity = OwningPlayer->GetVelocity();
	LocomotionBlending = Velocity.Size2D();
}
