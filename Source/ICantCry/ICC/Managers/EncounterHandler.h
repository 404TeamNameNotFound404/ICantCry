// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncounterManager.h"
#include "GameFramework/Actor.h"
#include "EncounterHandler.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AEncounterHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEncounterHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UEncounterManager* EncounterManager = nullptr;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Encounter", meta = (AllowPrivateAccess = "true"))
	EPlayerLocation MultiplierLocation;
	
};
