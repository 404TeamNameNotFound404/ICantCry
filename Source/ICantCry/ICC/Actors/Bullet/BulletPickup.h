// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Bullet/Bullet.h"
#include "../Source/ICantCry/ICC/Managers/InventoryManager.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Components/SphereComponent.h"
#include "BulletPickup.generated.h"

UCLASS()
class ICANTCRY_API ABulletPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    FBullet BulletData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    int32 Quantity = 1;

    UFUNCTION()
    void OnOverlapBegin( UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult);

};
