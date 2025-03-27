// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/Player/Camera/CameraWaypoint.h"
#include "WorldCamera.generated.h"

class AICC_Player;

UCLASS(Blueprintable)
class ICANTCRY_API AWorldCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Move the camera to the next waypoint statically (no distance interpolation)
	 */
	void MoveToNextWaypoint();
	
	bool IsDefaultCamera();
	void SetbDefaultCamera(const bool& bCondition);
	int GetWaypointIndex() const;
	void SetWaypointIndex(int index);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TArray<ACameraWaypoint*> Waypoints;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	int WaypointIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bDefaultCamera = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	AICC_Player* Player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bEnableSmoothMovement = false;

	UPROPERTY()
	AActor* CurrentWaypoint;

	UPROPERTY()
	bool bLerpStarted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CameraSmoothBlending = 6.0f;

	UPROPERTY()
	AActor* PreviousWaypoint;

	AActor* FindClosestWaypoint();
};
