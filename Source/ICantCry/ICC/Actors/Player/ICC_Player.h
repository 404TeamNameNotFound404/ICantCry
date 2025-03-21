// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Input/DataAssets/ICC_InputDataAsset.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ICC_Player.generated.h"

class UEncounterManager;

UCLASS()
class ICANTCRY_API AICC_Player : public AICC_Actor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AICC_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	int32 GetStepCounter() const { return StepCounter; } // Ottiene il numero di passi
    bool IsSprinting() const { return bIsSprinting; } // Controlla se sta correndo

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float SprintMultiplier = 1.5f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float MouseSensibility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector DirectionMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Body", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body", meta = (AllowPrivateAccess = "true"))
	UICC_InputDataAsset* InputDataAsset;


	void Input_Move(const FInputActionValue& InputActionValue);


	UPROPERTY()
    class UEncounterManager* EncounterManager;

	int32 StepCounter = 0;   // STEPS DONE
    bool bIsMoving = false;
	bool bWasMoving;
    bool bIsSprinting = false;

	void HandleMovement();
	void StartSprint();
	void StopSprint();

	

};
