// Fill out your copyright notice in the Description page of Project Settings.
#include "ICC_Player.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EntitySystem/MovieSceneEntityManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "ICantCry/ICC/Input/ICC_EnhancedInputCmp.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Mechanics/ChallengeMinigames/ChallengeMinigame.h"

// Sets default values
AICC_Player::AICC_Player()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	WalkSpeed = 500.0f;
	MouseSensibility = 0.2f;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(90.0f);
	GetCapsuleComponent()->SetCapsuleSize(90.0f, 200.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
	
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->ProbeChannel = ECC_Visibility;
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bDoCollisionTest = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 190.0f, 0.0f);
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	MinigameHandler = nullptr;
}

// Called when the game starts or when spawned
void AICC_Player::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCapsuleComponent()->SetCapsuleRadius(90.0f);
	GetCapsuleComponent()->SetCapsuleSize(90.0f, 200.0f);
	OldSpeed = GetCharacterMovement()->MaxWalkSpeed;
	
	InGameMenu = CreateWidget<UInGameMenu>(GetWorld(), InGameMenuClass);
	InGameMenu->AddToViewport();
	InGameMenu->SetVisibility(ESlateVisibility::Hidden);

	InventoryManager = NewObject<UInventoryManager>(this);
	InventoryManager->Initialize(InventoryHUD);
	InventoryManager->LinkCraftingHUD(CraftingHUD);
	InGameMenu->InstantiateTable(this);

	if (!WorldCamera)
	{
		for (TActorIterator<AWorldCamera> It(GetWorld()); It; ++It)
		{
			WorldCamera = *It;
			DebugHelper::LogMessage(3, FColor::Green, "WorldCamera found: " + WorldCamera->GetName());
			break;
		}
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UICantCryGameInstance* DontDestroyOnLoad = Cast<UICantCryGameInstance>(GameInstance);
	checkf(DontDestroyOnLoad, TEXT("Dontdestroyonload is invalid at player begin play"));

	// detect if battle handler are in the scene

	for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
	{
		MinigameHandler = *It;
		DebugHelper::LogSuccess("Minigame Handler found!");
		break;
	}
	
	DontDestroyOnLoad->SetPlayerStats(Stats);
	//DontDestroyOnLoad->GetInventory().StarterPack(); 
	DontDestroyOnLoad->SetPersistentPlayer(this);
	DontDestroyOnLoad->GetPersistentData()->InitialAttackPower = GetStats()->RuntimeStats.AttackPower;
	DontDestroyOnLoad->GetPersistentData()->InitialDefencePower = GetStats()->RuntimeStats.DefencePower;
	//bIsInFight = false;

	BestiaryUI= CreateWidget<UBestiaryUI>(GetWorld(), BestiaryUIClass);
	BestiaryUI->AddToViewport();
	BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
}

// Called every frame
void AICC_Player::Tick(float DeltaTime)
{
	if (!IsAlive())
	{
		return;
	}

	Super::Tick(DeltaTime);

	// --- Sistema di conteggio passi ---
	const FVector CurrentLocation = GetActorLocation();
	const float CurrentSpeed = GetVelocity().Size();

	// Se il player è in movimento
	if (CurrentSpeed > 0)
	{
		const float DistanceMoved = FVector::Dist(PreviousLocation, CurrentLocation);
		StepDistanceAccumulator += DistanceMoved;

		const float StepThreshold = 100.0f; // 1 meter per step

		if (StepDistanceAccumulator >= StepThreshold)
		{
			int32 StepsTaken = FMath::FloorToInt(StepDistanceAccumulator / StepThreshold);
			StepCounter += StepsTaken;
			StepDistanceAccumulator -= StepsTaken * StepThreshold;

			DebugHelper::LogSuccess(FString::Printf(TEXT("Steps taken: %d, Total steps: %d"), StepsTaken, StepCounter));
		}
	}

	PreviousLocation = CurrentLocation;


	// // Aggiorna sempre la posizione precedente
	// PreviousLocation = CurrentLocation;
}

// Called to bind functionality to input
void AICC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputDataAsset, TEXT("Forgot to add data asset to Blueprint"));
	ULocalPlayer* LocalPlayer = GetController<AICC_PlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	checkf(SubSystem, TEXT("Subsystem not found"));
	SubSystem->AddMappingContext(InputDataAsset->DefaultMappingContext, 0);
	UICC_EnhancedInputCmp* LastChecked = CastChecked<UICC_EnhancedInputCmp>(PlayerInputComponent);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Run, ETriggerEvent::Triggered, this, &ThisClass::Input_Run);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Interact, ETriggerEvent::Triggered, this, &ThisClass::Input_Interact);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Minigame, ETriggerEvent::Triggered, this, &ThisClass::Input_Minigame);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_BulletScroll, ETriggerEvent::Triggered, this, &ThisClass::Input_Scroll);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Inventory, ETriggerEvent::Triggered, this, &ThisClass::Input_OpenInventory);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Crafting, ETriggerEvent::Started, this, &ThisClass::Input_OpenCrafting);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_CloseCrafting, ETriggerEvent::Triggered, this, &ThisClass::Input_CloseCrafting);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_LMBInteract, ETriggerEvent::Triggered, this, &ThisClass::Input_ChallengeInteraction);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_LMBInteract, ETriggerEvent::Completed, this, &ThisClass::Input_ChallengeReleaseInteraction);
	//  LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Bestiary, ETriggerEvent::Started, this, &ThisClass::Input_OpenBestiary);
	// LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_CloseBestiary, ETriggerEvent::Triggered, this, &ThisClass::Input_CloseBestiary);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Bestiary, ETriggerEvent::Started, this, &ThisClass::Input_ToggleBestiary);
}


int AICC_Player::GetSpeed() const
{
	return Stats->Priority;
}

AWorldCamera* AICC_Player::GetWorldCamera() const
{  
    return WorldCamera;
}

UCameraComponent* AICC_Player::GetCamera() const
{
	return Camera;
}

bool AICC_Player::IsAlive() const
{
	return Stats->CurrentHealth > 0;
}

void AICC_Player::EnableMinigameInput(const bool& Enable)
{
	bEnableInputToMinigame = Enable;
	DebugHelper::LogMessage(3, FColor::Orange, "EnableMinigameInput: " + FString::FromInt(Enable));
}

void AICC_Player::SetActiveMinigameUserWidget(UMinigameUserWidget* Minigame)
{
	CurrentMinigameDisplayed = Minigame;
}

UBattleHUD* AICC_Player::GetBattleHUD() const
{
	return Hud;
}

UMinigameUserWidget* AICC_Player::GetCurrentMinigameDisplayed() const
{
	if (!CurrentMinigameDisplayed)
	{
		DebugHelper::LogError("Can't return the CurrentMinigameDisplayed since it's null");
		return nullptr;
	}
	
	return CurrentMinigameDisplayed;
}

AMinigameHandler* AICC_Player::GetMinigameHandler() const
{
	return MinigameHandler;
}

UPlayerStats* AICC_Player::GetStats() const
{
	return Stats;
}


UBattleData* AICC_Player::GetBattleData() const
{
	return BattleData;
}

int32 AICC_Player::GetWorldCameraCounter() const
{
	return CameraCounter;
}

void AICC_Player::SetWorldCameraCounter(const int32& Counter)
{
	CameraCounter = Counter;
}

const FInventory AICC_Player::GetPlayerInventory() const
{
    return PlayerInventory;
}

void AICC_Player::SetPlayerInventory(const FInventory &Inventory)
{
	PlayerInventory = Inventory;
}

UInventoryManager *AICC_Player::GetInventoryManager() const
{
    return InventoryManager;
}

UInGameMenu* AICC_Player::GetInGameMenu() const
{
	return InGameMenu;
}

UInventoryHUD* AICC_Player::GetInventoryHUD() const
{
	return InventoryHUD;
}


void AICC_Player::SetIsPickedUp(const bool& IsPicked)
{
	bReadyToPickUp = IsPicked;
}

bool AICC_Player::IsPickedUp() const
{
	return bReadyToPickUp;
}


void AICC_Player::Input_Move(const FInputActionValue& InputActionValue)
{
	if (bIsInFight) // if player is in fight don't move freely
	{
		return;
	}
	
	const FVector2d Direction = InputActionValue.Get<FVector2d>();
	DirectionMovement = FVector::ZeroVector;
	const FRotator Rotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = OldSpeed;
	
	if (Direction.Y != 0.f)
	{
		const FVector ForwardDirection = Rotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, Direction.Y);
		DirectionMovement.Y = Direction.Y;
	}
	
	if (Direction.X != 0.f)
	{
		const FVector RightDirection = Rotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, Direction.X);
		DirectionMovement.X = Direction.X;
	}
}



void AICC_Player::Input_Run(const FInputActionValue& InputActionValue)
{
	if (bIsInFight)
	{
		return;
	}
	
	const bool Pressed = InputActionValue.Get<bool>();
	
	if (Pressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void AICC_Player::Input_Minigame(const FInputActionValue& InputActionValue)
{
	if (!bEnableInputToMinigame)
	{
		return;
	}

	if (const bool Pressed = InputActionValue.Get<bool>() && CurrentMinigameDisplayed)
	{
		DebugHelper::LogSuccess("Minigame input enabled and i pressed P");
		CurrentMinigameDisplayed->SetStopSlider(true);
		CurrentMinigameDisplayed->Flow();
		
		MinigameHandler->EndMinigame();
	}
	else
	{
		DebugHelper::LogError("Can't init Minigame");
	}
}


void AICC_Player::Input_Interact(const FInputActionValue& InputActionValue)
{
	if (bIsInFight)
	{
		return;
	}
	
	DebugHelper::LogSuccess("Interacting with something");
}


void AICC_Player::Input_Scroll(const FInputActionValue &InputActionValue)
{
	// if player is not fighting there's no need to call this bindings
	if (!bIsInFight)
	{
		DebugHelper::LogError("Can't get scroll is in fight = false");
		return;
	}

	// if (!Hud->IsSelectingTarget())
	// {
	// 	DebugHelper::LogError("You can't do that yet");
	// 	return;
	// }
	
	DebugHelper::LogSuccess("Scrolling something");
	
	if(!Hud)
	{
		DebugHelper::LogError("HUD is null!");
		return;
	}
	
	const float Scroll = InputActionValue.Get<float>();

	if (Hud->GetSelectTarget())
	{
		Hud->ScrollTargetSelection(Scroll);
		DebugHelper::LogSuccess("Selecting target ");
	}
	else
	{
		if(Scroll && !Hud->IsBulletSelectionOver())
		{
			DebugHelper::LogSuccess("Scrolling over bullets ");
			Hud->ScrollBulletSelection(Scroll);
		}
	}
}

void AICC_Player::Input_OpenInventory(const FInputActionValue& InputActionValue)
{
	ToggleInventory();
}

void AICC_Player::CloseInventory()
{
}

void AICC_Player::ToggleInventory()
{
	if (!InventoryHUD)
	{
		InventoryHUD = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDClass);

		if (InventoryHUD)
		{
			InventoryHUD->AddToViewport();

			InventoryHUD->UpdateInventoryDisplay(PlayerInventory); // aggiorna manualmente

			AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController());
			if (PC)
			{
				PC->SetInputMode(FInputModeUIOnly());
				PC->bShowMouseCursor = true;
			}
		}
		return;
	}

	if (InventoryHUD->IsInViewport())
	{
		CloseInventory();
	}
	else
	{
		InventoryHUD->AddToViewport();
		InventoryHUD->UpdateInventoryDisplay(PlayerInventory); //  aggiorna quando riapri

		AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController());
		if (PC)
		{
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;
		}
	}
}

void AICC_Player::ToggleCraftingHUD()
{
	InGameMenu->SetMenuOpen(true);
	InGameMenu->SetVisibility(ESlateVisibility::Visible);
	//CraftingHUD->RefreshUI();
	Cast<AICC_PlayerController>(GetController())->bShowMouseCursor = true;
}

void AICC_Player::CloseCraftingHUD()
{
	Cast<AICC_PlayerController>(GetController())->bShowMouseCursor = false;
	InGameMenu->SetMenuOpen(false);
	InGameMenu->SetVisibility(ESlateVisibility::Hidden);
	//CraftingHUD->SetVisibility(ESlateVisibility::Hidden);
	CraftingCounter = 0;
}

void AICC_Player::Input_OpenCrafting(const FInputActionValue& InputActionValue)
{
	if (InGameMenu->IsDisabled())
	{
		return;
	}
	
	if (const bool Pressed = InputActionValue.Get<bool>() && CraftingCounter == 0)
	{
		DebugHelper::LogSuccess("Opening Crafting");
		InGameMenu->SetMenuOpen(true);
		//bIsInGameMenuOpen = true;
		ToggleCraftingHUD();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			CraftingCounter = 1;
		}, 0.1f, false);
	}
}

void AICC_Player::Input_CloseCrafting(const FInputActionValue& InputActionValue)
{
	if (const bool Pressed = InputActionValue.Get<bool>() && CraftingCounter == 1 && InGameMenu->IsOpen())
	{
		DebugHelper::LogSuccess("Closing Crafting");
		CloseCraftingHUD();
		CraftingCounter = 0;
		//bIsInGameMenuOpen = false;
		InGameMenu->SetMenuOpen(false);
	}
}

void AICC_Player::Input_ChallengeInteraction(const FInputActionValue& InputActionValue)
{
	if (bIsInFight || !AChallengeMinigame::Singleton)
	{
		return;
	}

	if (AChallengeMinigame::Singleton->GetIsInArea())
	{
		AChallengeMinigame::Singleton->PickPaper();
	}
}

int32 AICC_Player::GetStepCounter() const
{
    return StepCounter;
}

bool AICC_Player::IsSprinting() const
{
    return GetCharacterMovement()->MaxWalkSpeed == RunSpeed;
}

void AICC_Player::ResetStepCounter()
{
    StepCounter = 0;
    StepDistanceAccumulator = 0.0f;
}

// BESTIARY
void AICC_Player::CollectNote(const FString& NoteKey)
{
   if (BestiaryUI)
    {
        BestiaryUI->AddCollectedNote(NoteKey);
    }
    else
    {
        // Salva temporaneamente e aggiungi quando BestiaryUI è disponibile
       // PendingNotes.Add(NoteKey);
    }
}


void AICC_Player::Input_OpenBestiary(const FInputActionValue& InputActionValue)
{
	if (bIsInFight)
	{
		return;
	}

	const bool Pressed = InputActionValue.Get<bool>();
	if (Pressed && BestiaryCounter == 0)
	{
		DebugHelper::LogSuccess("Opening Bestiary");
		OpenBestiary();
		BestiaryUI->SetIsOpen(true);
		BestiaryUI->SetVisibility(ESlateVisibility::Visible);

		BestiaryCounter = 1; // set immediately, no timer needed
	}
}

void AICC_Player::Input_CloseBestiary(const FInputActionValue& InputActionValue)
{
	const bool Pressed = InputActionValue.Get<bool>();
	if (Pressed && BestiaryCounter == 1)
	{
		DebugHelper::LogSuccess("Closing Bestiary");
		CloseBestiary();
		BestiaryCounter = 0;
		BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
		BestiaryUI->SetIsOpen(false);
		DebugHelper::LogSuccess("Bestiary Closed Successfully");
	}
}


void AICC_Player::OpenBestiary()
{
	BestiaryUI->SetVisibility(ESlateVisibility::Visible);

	if (AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController()))
	{
		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		Mode.SetWidgetToFocus(BestiaryUI->TakeWidget()); // optional; gives focus but still lets game receive input
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;
	}

	BestiaryUI->SetIsOpen(true);
	DebugHelper::LogSuccess("Bestiary opened");
}


void AICC_Player::CloseBestiary()
{
	BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
	BestiaryUI->SetIsOpen(false);

	if (AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	DebugHelper::LogSuccess("Bestiary closed");
}

void AICC_Player::Input_ToggleBestiary(const FInputActionValue& InputActionValue)
{
	if (!InputActionValue.Get<bool>()) return;

	if (BestiaryUI->IsVisible())
	{
		CloseBestiary();
	}
	else
	{
		OpenBestiary();
	}
}
































void AICC_Player::Input_ChallengeReleaseInteraction(const FInputActionValue& InputActionValue)
{
	if (AChallengeMinigame::Singleton)
	{
		if (APaper* Paper = AChallengeMinigame::Singleton->GetCurrentPaper())
		{
			Paper->Release();
		}
	}
}