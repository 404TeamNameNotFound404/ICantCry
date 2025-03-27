/*
 *                                  I Can't Cry Project
 *                  Authors: BPA98 - Graaced
 *                  
 */ 

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Input/DataAssets/ICC_InputDataAsset.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ICantCry/ICC/Actors/Player/Camera/WorldCamera.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICC_Player.generated.h"

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

	/**
	 * READ BELOW BEFORE USE!
	 * @note This is the Static World Camera Reference not the Internal Player camera!
	 *  to get the player camera you may want to call GetCamera
	 * @return World Static Camera
	 */
	AWorldCamera* GetWorldCamera() const;

	/**
	 * Get the Camera Component
	 * @return Player Camera
	 */
	UCameraComponent* GetCamera() const;
	
	/**
 * Deny the player movement during the fight (will be controller later with Battle Action)
 * @note Change back EditAnywhere to EditDefaultsOnlye and BlueprintReadWrite to BlueprintReadOnly. This is just
 * for debugging stage
 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle System", Blueprintable)
	bool bIsInFight = false;
	

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float MouseSensibility;

	UPROPERTY()
	float OldSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector DirectionMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	AWorldCamera* WorldCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Body", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body", meta = (AllowPrivateAccess = "true"))
	UICC_InputDataAsset* InputDataAsset;

	UPROPERTY()
	AMinigameHandler* MinigameHandler;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Interact(const FInputActionValue& InputActionValue);
	void Input_Run(const FInputActionValue& InputActionValue);
	void Input_MinigameInteract(const FInputActionValue& InputActionValue);
};
