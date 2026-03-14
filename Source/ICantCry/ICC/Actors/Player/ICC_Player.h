/*
 *                                  I Can't Cry Project
 *                  Authors: BPA98 - Graaced
 *                  
 */ 

#pragma once

#include "CoreMinimal.h"
#include "ICC_PlayerMemory.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Input/DataAssets/ICC_InputDataAsset.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ICantCry/ICC/Actors/Player/Camera/WorldCamera.h"
#include "ICantCry/ICC/Input/Gamepad/ICC_GamepadBinder.h"
#include "ICantCry/ICC/Managers/InventoryManager.h"
#include "ICantCry/ICC/Mechanics/Core/Data/BattleData.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "ICantCry/ICC/UI/InGameMenu.h"
#include "ICantCry/ICC/UI/BattleHUD.h"
#include "ICantCry/ICC/UI/BestiaryUI.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
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

	virtual int GetSpeed() const override;

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

	bool IsAlive() const;

	bool GetIsMinigameInputEnabled() const;
	void EnableMinigameInput(const bool& Enable);
	void SetActiveMinigameUserWidget(UMinigameUserWidget* Minigame);

	UBattleHUD* GetBattleHUD() const;
	UMinigameUserWidget* GetCurrentMinigameDisplayed() const;
	AMinigameHandler* GetMinigameHandler() const;
	UPlayerStats* GetStats() const;
	/**
 * Battle Data simple data asset used to edit buff and debuff increment values 
 * 
 * @return Battle Data
 */
	UBattleData* GetBattleData() const;

	const FInventory GetPlayerInventory() const;
	void SetPlayerInventory(const FInventory& Inventory);
	UInventoryManager* GetInventoryManager() const;

	/**
	 * Read below!!
	 * @note DEBUG ONLY!
	 */
	UPROPERTY(EditAnywhere, Blueprintreadwrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DebugMesh;

	/**
	 * Checks the internal 'Counter' of Hallway Traveler class.
	 * Meaning, if I pass through an instance and after that I pass into another instance ,
	 * the counter won't get updated so it will be stuck at the previous waypoint.
	 * This counter fixes the issue
	 */
	int32 GetWorldCameraCounter() const;

	/**
	 * Set the global counter equal to the internal counter
	 * @note DO NOT SET ANYWHERE ELSE
	 * @param Counter Internal counter parameter
	 */
	void SetWorldCameraCounter(const int32& Counter);

	void SetIsPickedUp(const bool& IsPicked);
	bool IsPickedUp() const;
	
	
	UInGameMenu* GetInGameMenu() const;
	UInventoryHUD* GetInventoryHUD() const;

	UFUNCTION()
	int32 GetStepCounter() const;

	UFUNCTION()
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResetStepCounter();

	UFUNCTION(BlueprintCallable,  Category = "Level System")
	float GetExpRequiredForNextLevel()  const;

	UFUNCTION(BlueprintCallable,  Category = "Level System")
	float GetCurrentExpPercentage()  const;

	UICC_GamepadBinder* GetBinder() const;
	USpringArmComponent* GetCameraBoom() const;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	UPlayerStats* Stats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	UBattleData* BattleData;

	UPROPERTY()
	AMinigameHandler* MinigameHandler;

	UPROPERTY()
	bool bEnableInputToMinigame = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UBattleHUD* Hud;

	UPROPERTY()
	UICC_GamepadBinder* PadBinder;

	UPROPERTY()
	UMinigameUserWidget* CurrentMinigameDisplayed = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UInventoryHUD* InventoryHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UInventoryManager* InventoryManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventoryHUD> InventoryHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInGameMenu> InGameMenuClass;
	
	UPROPERTY()
	UInGameMenu* InGameMenu;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UCraftingHUD* CraftingHUD;  

	UPROPERTY()
	UCraftingTable* CraftingTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCraftingHUD> CraftingHUDClass;

	UPROPERTY()
	FInventory PlayerInventory;

	UPROPERTY()
	int32 CameraCounter = 0;

	UPROPERTY()
	bool bReadyToPickUp;

	/**
 * Encounter Manager
 */
	UPROPERTY()
	FVector PreviousLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Movement")
	float StepDistanceAccumulator = 0.0f;

	UPROPERTY()
	int32 StepCounter = 0;


	/**
 * Close the crafting / inventory counter
 */
	UPROPERTY()
	int32 CraftingCounter;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Interact(const FInputActionValue& InputActionValue);
	void Input_Run(const FInputActionValue& InputActionValue);
	void Input_Minigame(const FInputActionValue& InputActionValue);
	void Input_MinigameReleased(const FInputActionValue& InputActionValue);
	void Input_Scroll(const FInputActionValue& InputActionValue);
	void Input_OpenInventory(const FInputActionValue& InputActionValue);
	void Input_OpenCrafting(const FInputActionValue& InputActionValue);
	void Input_CloseCrafting(const FInputActionValue &InputActionValue);
	void Input_ChallengeInteraction(const FInputActionValue &InputActionValue); // Puzzle assemble minigame interaction
	void Input_OpenBestiary(const FInputActionValue& InputActionValue);
	void Input_CloseBestiary(const FInputActionValue& InputActionValue);
	void Input_ToggleBestiary(const FInputActionValue& InputActionValue);
	void Input_ChallengeReleaseInteraction(const FInputActionValue& InputActionValue);

	void CloseInventory();
	void ToggleInventory();
	void ToggleCraftingHUD();
	void CloseCraftingHUD();
	


	//Bestiary 
	void CollectNote(const FString& NoteKey); // da cancellare
	void OpenBestiary();
    void CloseBestiary();

	UPROPERTY() 
    int32 BestiaryCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UBestiaryUI* BestiaryUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UBestiaryUI> BestiaryUIClass;


};
