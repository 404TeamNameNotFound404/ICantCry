// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "EAnxiety.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AEAnxiety : public AMob
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEAnxiety();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
