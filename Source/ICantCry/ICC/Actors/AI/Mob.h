// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "Mob.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AMob : public AICC_Actor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMob();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
