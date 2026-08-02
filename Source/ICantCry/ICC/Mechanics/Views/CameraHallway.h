// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Views/CameraTraveler.h"
#include "ICantCry/ICC/Mechanics/Views/PlayerTeleportObj.h"
#include "ICantCry/ICC/Mechanics/Views/ICCRoomHandler.h"
#include "CameraHallway.generated.h"

UCLASS()
class ICANTCRY_API ACameraHallway : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraHallway();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Traveler", meta=(AllowPrivateAccess="true"))
	TObjectPtr<AICCRoomHandler> RoomHandler;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Traveler", meta=(AllowPrivateAccess="true"))
	bool bEnableFade = true;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,  int32 OtherBodyIndex,  
				bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

	UPROPERTY()
	bool bPlayerOverlapped = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly,  Category = "Traveler" ,meta=(AllowPrivateAccess = "true"))
	bool bSnap = false;
	
	UPROPERTY()
	int Counter = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	AICC_Player* Player;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	TArray<ACameraTraveler*> Travelers;

 	/**
	 * Entry Waypoint (first to travel)
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	ACameraWaypoint* CameraFixedWaypoint;

	/**
	 * Return Waypoint
	 * Previous 
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	ACameraWaypoint* CameraBackWaypoint;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	bool bPlayerMustTeleport = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<APlayerTeleportObj> InEntry;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<APlayerTeleportObj> OutEntry;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	bool bEnableWorldCamera = false;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	bool bEnableWorldCameraOnExit;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	float CameraBlendSpeed = 0.5f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	FName RoomId;

	void Snap();
	
	void ToggleRoom(const FName& RoomTag);
};
