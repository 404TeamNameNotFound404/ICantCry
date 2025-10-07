// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Paper.generated.h"

UCLASS(Blueprintable, Abstract)
class ICANTCRY_API APaper : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APaper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	ACameraActor* MinigameCamera;

	UPROPERTY() bool bLifted = false;
	UPROPERTY() bool bIsSwapped = false;
	UPROPERTY() bool bIsBeingDragged = false;
	UPROPERTY() FVector StartLocation;
	UPROPERTY() FVector CurrentLocation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Lift(const FVector& LiftLocation);
	virtual void Release();

	FVector GetStartLocation() const;
	FVector GetCurrentLocation() const;
	bool GetIsLifted() const;
	bool GetIsSwapped() const;
	void SetIsDragged(const float& Value);
};
