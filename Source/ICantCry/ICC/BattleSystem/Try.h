// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Managers/EncounterManager.h"
#include "Try.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API ATry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATry();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true") )
	UEncounterManager* EncounterManager;
};
