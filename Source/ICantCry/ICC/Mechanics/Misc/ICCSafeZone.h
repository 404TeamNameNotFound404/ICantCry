// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Managers/EncounterHandler.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICCSafeZone.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AICCSafeZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AICCSafeZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "SafeZone", meta=(AllowPrivateAccess = "true"))
	UBoxComponent* SafeArea;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "SafeZone", meta=(AllowPrivateAccess = "true"))
	bool bInSafeArea;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="SafeZone", meta=(AllowPrivateAccess="true"))
	AEncounterHandler* Handler;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
	UPROPERTY()
	UICantCryGameInstance* Instance;

public:

	bool IsPlayerInSafeZone() const;
};
