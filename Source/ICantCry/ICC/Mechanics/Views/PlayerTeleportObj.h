// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerTeleportObj.generated.h"

UCLASS()
class ICANTCRY_API APlayerTeleportObj : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerTeleportObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
