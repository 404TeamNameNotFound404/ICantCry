// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Managers/InventoryManager.h"
#include "../Source/ICantCry/ICC/Inventory/Inventory.h"
#include "ICantCry/ICC/Actors/Bullet/Essence.h"
#include "Components/SphereComponent.h"
#include "EssencePickup.generated.h"

UCLASS()
class ICANTCRY_API AEssencePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEssencePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Essence")
    EEssenceType EssenceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Essence")
    int32 Quantity = 1;

	UPROPERTY()
	FEssence Self;

    
    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

};
