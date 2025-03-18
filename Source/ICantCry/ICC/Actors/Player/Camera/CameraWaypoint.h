// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraWaypoint.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API ACameraWaypoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraWaypoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
