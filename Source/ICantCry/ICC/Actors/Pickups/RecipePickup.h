// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Source/ICantCry/ICC/Inventory/Recipe.h"
#include "../Source/ICantCry/ICC/Managers/InventoryManager.h"
#include "ICantCry/ICC/Inventory/Inventory.h"
#include "RecipePickup.generated.h"


class UStaticMeshComponent;
class USphereComponent;
class AICC_Player;

UCLASS()
class ICANTCRY_API ARecipePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARecipePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	 UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    ERecipeType RecipeType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	FRecipe Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	int32 Quantity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* Collision;

	UFUNCTION()
    void OnPickedUp(class AActor* OtherActor);

};
