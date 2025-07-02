// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Actors/Bullet/GoldEmptyCasing.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "GoldEmptyCasingPickup.generated.h"

UCLASS()
class ICANTCRY_API AGoldEmptyCasingPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGoldEmptyCasingPickup();

	FGoldEmptyCasing& GetSelf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickups", meta = (AllowPrivateAccess = "true"))
	FGoldEmptyCasing Self;

private:


	UPROPERTY()
	UBoxComponent* BoxCollision;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,  int32 OtherBodyIndex,  
				bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	int32 QuantityCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickups", meta = (AllowPrivateAccess = "true"))
	ECasingType CasingType;
	
	void OnPickUp();
};
