// Fill out your copyright notice in the Description page of Project Settings.
#include "ICC_Player.h"
#include "EngineUtils.h"
#include "CollisionQueryParams.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EntitySystem/MovieSceneEntityManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetInteractionComponent.h"


#include "ICantCry/ICC/Input/ICC_EnhancedInputCmp.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"
#include "ICantCry/ICC/Mechanics/ChallengeMinigames/ChallengeMinigame.h"
#include "ICantCry/ICC/Narrative/Components/InteractionComponent.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Pickups/BasePickup.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/UI/BarkWidget.h" 


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

	PadBinder = CreateDefaultSubobject<UICC_GamepadBinder>(TEXT("GamepadBinder"));
	
	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	if (WidgetInteractionComp)
	{
		// Imposta un indice utente virtuale per isolare l'input
		WidgetInteractionComp->VirtualUserIndex = 99;
		// Impedisci che interagisca con il focus hardware
		WidgetInteractionComp->bShowDebug = false;
	}
}

// Called when the game starts or when spawned
void AICC_Player::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	
	for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
	{
		MinigameHandler = *It;
		break;
	}
	
	DontDestroyOnLoad->SetPlayerStats(Stats);
	DontDestroyOnLoad->SetPersistentPlayer(this);

	DontDestroyOnLoad->GetPersistentData()->InitialAttackPower = DontDestroyOnLoad->GetRuntimeStats().AttackPower;
	DontDestroyOnLoad->GetPersistentData()->InitialDefencePower = DontDestroyOnLoad->GetRuntimeStats().DefencePower;
	
	if (!DontDestroyOnLoad->bHealthInitialized)
	{
		DontDestroyOnLoad->GetRuntimeStats().CurrentHealth = DontDestroyOnLoad->GetPlayerStats()->MaxHealth;
		DontDestroyOnLoad->bHealthInitialized = true;
	}
	
	DebugHelper::LogMessage(5, FColor::Cyan, FString::Printf(TEXT("Health on Spawn: %f"), DontDestroyOnLoad->GetRuntimeStats().CurrentHealth));

	BestiaryUI= CreateWidget<UBestiaryUI>(GetWorld(), BestiaryUIClass);
	BestiaryUI->AddToViewport();
	BestiaryUI->SetVisibility(ESlateVisibility::Hidden);

	if (UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance()))
	{
		GI->ActiveBestiaryUI = BestiaryUI;
	}
	
	PadBinder->Init(this);

	if (WidgetInteractionComp)
	{
		WidgetInteractionComp->RegisterComponent();
		WidgetInteractionComp->Activate();
	}
}

// Called every frame
void AICC_Player::Tick(float DeltaTime)
{
	if (!IsAlive())
	{
		return;
	}

	Super::Tick(DeltaTime);

	// Step counting system 
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
		}
	}

	PreviousLocation = CurrentLocation;

	// LOGIC OF THE VISUAL SCANNER

	UInteractionComponent* FoundTarget = ScanForInteractables();

	// Se abbiamo cambiato bersaglio (o lo abbiamo perso)
	if (FoundTarget != CurrentInteractableTarget)
	{
		if (CurrentInteractableTarget)
		{
			CurrentInteractableTarget->HideAllInteractionUI();
		}
		CurrentInteractableTarget = FoundTarget;
		LookTimer = 0.0f; // Resetta il timer anti-flicker
	}

	// Se stiamo guardando validamente un oggetto
	if (CurrentInteractableTarget)
	{
		float Distance = FVector::Dist(GetActorLocation(), CurrentInteractableTarget->GetOwner()->GetActorLocation());

		//if (Distance <= InteractableRadius)
		//{
		//	LookTimer += DeltaTime;
		//	// Se lo stiamo guardando da più di 0.2 secondi e siamo vicini, mostriamo il tasto "E"
		//	if (LookTimer >= 0.2f)
		//	{
		//		CurrentInteractableTarget->ShowInteractButtonPrompt();
		//	}
		//	else
		//	{
		//		// Nel frattempo mostriamo solo il cerchietto
		//		CurrentInteractableTarget->ShowDistantCircleIcon();
		//	}
		//}
		//else
		//{
		//	// Siamo lontani, mostriamo solo il cerchietto bianco
		//	CurrentInteractableTarget->ShowDistantCircleIcon();
		//	LookTimer = 0.0f;
		//}

		//float EffectiveRadius = (CurrentInteractableDistance <= InteractableRadius)
		//	? InteractableRadius + 20.0f  // se già dentro, serve uscire di più per uscire
		//	: InteractableRadius;         // se fuori, serve entrare per entrare

		if (Distance <= InteractableRadius)
		{
			LookTimer += DeltaTime;
			if (LookTimer >= 0.2f)
				CurrentInteractableTarget->ShowInteractButtonPrompt();
			else
				CurrentInteractableTarget->ShowDistantCircleIcon();
		}
		else
		{
			CurrentInteractableTarget->ShowDistantCircleIcon();
			LookTimer = 0.0f;
		}

		CurrentInteractableDistance = Distance;

	}

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
	LastChecked = CastChecked<UICC_EnhancedInputCmp>(PlayerInputComponent);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Run, ETriggerEvent::Triggered, this, &ThisClass::Input_Run);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Interact, ETriggerEvent::Triggered, this, &ThisClass::Input_Interact);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Minigame, ETriggerEvent::Started, this, &ThisClass::Input_Minigame);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Minigame, ETriggerEvent::Completed, this, &ThisClass::Input_MinigameReleased);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_BulletScroll, ETriggerEvent::Triggered, this, &ThisClass::Input_Scroll);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Inventory, ETriggerEvent::Triggered, this, &ThisClass::Input_OpenInventory);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_Crafting, ETriggerEvent::Started, this, &ThisClass::Input_OpenCrafting);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_CloseCrafting, ETriggerEvent::Triggered, this, &ThisClass::Input_CloseCrafting);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_LMBInteract, ETriggerEvent::Triggered, this, &ThisClass::Input_ChallengeInteraction);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_LMBInteract, ETriggerEvent::Completed, this, &ThisClass::Input_ChallengeReleaseInteraction);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadInteraction, ETriggerEvent::Completed, PadBinder, &UICC_GamepadBinder::Input_GamepadEngageInteraction);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadInteraction, ETriggerEvent::Triggered, PadBinder, &UICC_GamepadBinder::Input_GamepadSelectionInteraction);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadRemoveBullet, ETriggerEvent::Triggered, PadBinder, &UICC_GamepadBinder::Input_GamepadRemoveBullet);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadShootBoost, ETriggerEvent::Triggered, PadBinder, &UICC_GamepadBinder::Input_GamepadShootBoost);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadNavigate, ETriggerEvent::Triggered, PadBinder, &UICC_GamepadBinder::Input_GamepadNavigateUi);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_GamepadSimulateClick, ETriggerEvent::Triggered, PadBinder, &UICC_GamepadBinder::Input_GamepadSimulateClick);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameGuitarHero_X, ETriggerEvent::Started, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_X);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameGuitarHero_Y, ETriggerEvent::Started, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_Y);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameGuitarHero_A, ETriggerEvent::Started, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_A);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameGuitarHero_B, ETriggerEvent::Started, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameGuitarHero_B);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameCurling, ETriggerEvent::Started, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameCurling);
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameCurling, ETriggerEvent::Completed | ETriggerEvent::None, PadBinder, &UICC_GamepadBinder::Input_GamepadMinigameCurlingRelease);
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
	return Cast<UICantCryGameInstance>(GetGameInstance())->GetRuntimeStats().CurrentHealth > 0;
}

bool AICC_Player::GetIsMinigameInputEnabled() const
{
	return bEnableInputToMinigame;
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
	
	

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	const float DeadZone = 0.2f;

	if (Controller && MovementVector.Size() > DeadZone) // Controller && (MovementVector.SizeSquared() > 0.0f)
	{
		// Trova la direzione Forward basata sulla rotazione della telecamera (Control Rotation)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// Calcola i vettori di direzione corretti nello spazio del mondo
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Applica l'input direttamente senza passare dal Tick
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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
	if (!bEnableInputToMinigame || !CurrentMinigameDisplayed)
	{
		return;
	}
	
	const float Value = InputActionValue.Get<float>();
	CurrentMinigameDisplayed->SetScrollValue(Value);
	
	if (Value <= 0.f)
	{
		return;
	}
	
	if (Value > 0.f && CurrentMinigameDisplayed)
	{
		CurrentMinigameDisplayed->Handle(GetBattleHUD()->GetCurrentBulletData(), GetMinigameHandler());
		CurrentMinigameDisplayed->Flow();
	}
	else
	{
		DebugHelper::LogError("Can't init Minigame");
	}
}

void AICC_Player::Input_MinigameReleased(const FInputActionValue& InputActionValue)
{
	PadBinder->Input_GamepadMinigameRelease(InputActionValue);
}

void AICC_Player::Input_Interact(const FInputActionValue& InputActionValue)
{
	
	if (bIsInFight) return;
	if (!InputActionValue.Get<bool>()) return;

	if (CurrentInteractableTarget)
	{
		DebugHelper::LogSuccess("I run Trigger on InteractionComponent from Scanner!");
		CurrentInteractableTarget->TriggerInteraction(this);
	}
}


void AICC_Player::Input_Scroll(const FInputActionValue &InputActionValue)
{
	// if player is not fighting there's no need to call this bindings
	if (!bIsInFight)
	{
		DebugHelper::LogError("Can't get scroll is in fight = false");
		return;
	}
	
	
	if(!Hud)
	{
		DebugHelper::LogError("HUD is null!");
		return;
	}
	
	const float Scroll = InputActionValue.Get<float>();

	if (FMath::IsNearlyZero(Scroll))
	{
		return;
	}
	
	if (Hud->IsBulletSelectionOver())
	{
		Hud->ScrollTargetSelection(Scroll);
		return;
	}
	if (PadBinder->IsNavigating())
	{
		Hud->ScrollBulletSelection(Scroll);
		return;
	}
	// else
	// {
	// 	Hud->ScrollBulletSelection(Scroll);
	// }
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

	// if (InventoryHUD->IsInViewport())
	// {
	// 	CloseInventory();
	// }
	// else
	// {
	// 	InventoryHUD->AddToViewport();
	// 	InventoryHUD->UpdateInventoryDisplay(PlayerInventory); //  aggiorna quando riapri
	//
	// 	AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController());
	// 	if (PC)
	// 	{
	// 		PC->SetInputMode(FInputModeUIOnly());
	// 		PC->bShowMouseCursor = true;
	// 	}
	// }

	if (InventoryHUD->IsVisible())
	{
		InventoryHUD->SetVisibility(ESlateVisibility::Hidden);
		CloseInventory();
	}
	else
	{
		InventoryHUD->SetVisibility(ESlateVisibility::Visible);
		InventoryHUD->UpdateInventoryDisplay(PlayerInventory);
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
	if (InGameMenu->IsDisabled() || bIsInFight)
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
	if (bIsInFight) return;
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
	if (BestiaryUI)
	{
		BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
		BestiaryUI->SetIsOpen(false);
	}

	if (AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	DebugHelper::LogSuccess("Bestiary closed");

	// Se c'è un bark in sospeso (es. dopo aver raccolto un ItemWithNote),
	// lo facciamo partire ORA che il Bestiario è stato chiuso. Una volta sola.
	if (PendingBark)
	{
		SpawnBark(PendingBark);
		PendingBark = nullptr;
	}

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


void AICC_Player::OpenBestiaryOnEntryAndListenForClose(const FString& EntryID, UDialogueAsset* BarkToPlay)
{
	//// 1. Memorizziamo il dialogo per riprodurlo DOPO la chiusura
	//PendingBark = BarkToPlay;

	//// 2. Apriamo fisicamente il menu a schermo
	//OpenBestiary();

	//if (BestiaryUI)
	//{
	//	BestiaryUI->SetIsOpen(true);
	//	// Qui, se vuoi che il bestiario si apra DIRETTAMENTE sulla pagina della nota appena raccolta,
	//	// dovrai chiamare la funzione del BestiaryUI che seleziona la nota. Ad esempio:
	//	// BestiaryUI->OnNoteSelected(EntryID); // *Attenzione: OnNoteSelected nel tuo codice attuale richiede un FString!
	//}

	PendingBark = BarkToPlay;

	
	OpenBestiary();

	if (BestiaryUI)
	{
		BestiaryUI->SetIsOpen(true);
		
		BestiaryUI->ForceOpenNoteByKey(EntryID);
	}
}

void AICC_Player::PlayBarkImmediately(UDialogueAsset* BarkToPlay)
{

	//if (!BarkToPlay) return;

	//
	//bool bIsAnyMenuOpen = (BestiaryUI && BestiaryUI->IsOpen()) || (InGameMenu && InGameMenu->IsOpen());

	//if (bIsAnyMenuOpen)
	//{
	//	// The menu is open! Let's not start the dialogue now.
	//	// i saving it so it will start automatically when the player closes the UI.
	//	PendingBark = BarkToPlay;
	//	DebugHelper::LogMessage(3, FColor::Yellow, "Menu aperto: Bark messo in coda.");
	//	return;
	//}

	//// If no menu is open (standard behavior for Quest Items), we start the dialogue
	//if (DialogueWidgetClass)
	//{
	//	UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
	//	if (DialogueWidget)
	//	{
	//		DialogueWidget->AddToViewport();
	//		DialogueWidget->StartDialogue(BarkToPlay);
	//	}
	//}

	if (!BarkToPlay) return;

	bool bIsAnyMenuOpen = (BestiaryUI && BestiaryUI->IsOpen()) || (InGameMenu && InGameMenu->IsOpen());
	if (bIsAnyMenuOpen)
	{
		// Il menu è aperto: non avviamo subito il bark.
		// Lo salviamo così parte automaticamente quando il player chiude la UI (in CloseBestiary).
		PendingBark = BarkToPlay;
		DebugHelper::LogMessage(3, FColor::Yellow, "Menu aperto: Bark messo in coda.");
		return;
	}

	// Nessun menu aperto: avvia subito il bark
	SpawnBark(BarkToPlay);
}






float AICC_Player::GetExpRequiredForNextLevel() const
{
	if(!Stats) return 100.0f; 
	
	constexpr float BaseExp = 100.0f; 
	
	const float Multiplier = 1.0f + (Cast<UICantCryGameInstance>(GetGameInstance())->GetRuntimeStats().Level * 0.5f);

	return BaseExp * Multiplier;
}


float AICC_Player::GetCurrentExpPercentage() const
{
    if(!Stats) return 0.0f;

	const float ExpRequired = GetExpRequiredForNextLevel();

	if(ExpRequired <= 0.0f) return 0.0f;

	return FMath::Clamp(Cast<UICantCryGameInstance>(GetGameInstance())->GetRuntimeStats().Experience / ExpRequired, 0.0f, 1.0f);
}

UICC_GamepadBinder* AICC_Player::GetBinder() const
{
	return PadBinder;
}

USpringArmComponent* AICC_Player::GetCameraBoom() const
{
	return CameraBoom;
}

FRuntimeStats& AICC_Player::GetRuntimeStats()
{
	return Cast<UICantCryGameInstance>(GetGameInstance())->GetRuntimeStats();
}

UICC_InputDataAsset* AICC_Player::GetInputDataAsset()
{
	return InputDataAsset;
}

UICC_EnhancedInputCmp* AICC_Player::GetInputBinder()
{
	return LastChecked;
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


UInteractionComponent* AICC_Player::ScanForInteractables()
{
	if (!Camera) return nullptr;

	FVector CameraLoc = Camera->GetComponentLocation();
	FVector CameraForward = Camera->GetForwardVector();

	UInteractionComponent* BestTarget = nullptr;
	float ClosestDot = -1.0f;

	// 1. Troviamo gli attori in un raggio attorno al player
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), DetectionRadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>(), AActor::StaticClass(), ActorsToIgnore, OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		UInteractionComponent* InteractComp = Actor->FindComponentByClass<UInteractionComponent>();

		// Se non ha il componente o se è una nota già letta (CanInteract() restituisce false), lo ignoriamo!
		if (!InteractComp || !InteractComp->CanInteract()) continue;

		FVector DirToTarget = (Actor->GetActorLocation() - CameraLoc).GetSafeNormal();
		float DotProduct = FVector::DotProduct(CameraForward, DirToTarget);

		// 2. L'oggetto è davanti a noi? (0.65f corrisponde a un cono visivo di circa 45 gradi)
		if (DotProduct >= 0.65f)
		{
			// 3. C'è un muro in mezzo? Facciamo un raycast per controllare
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, Actor->GetActorLocation(), ECC_Visibility, Params);

			if (bHit && Hit.GetActor() == Actor)
			{
				// Se ci sono più oggetti vicini, diamo la priorità a quello più centrato nello schermo
				if (DotProduct > ClosestDot)
				{
					ClosestDot = DotProduct;
					BestTarget = InteractComp;
				}
			}
		}
	}

	return BestTarget;
}


void AICC_Player::SpawnBark(UDialogueAsset* BarkToPlay)
{
	if (!BarkToPlay || !BarkWidgetClass) return;

	if (UBarkWidget* Bark = CreateWidget<UBarkWidget>(GetWorld(), BarkWidgetClass))
	{
		Bark->AddToViewport();
		Bark->StartBark(BarkToPlay);
	}
}

UICC_InputDataAsset* AICC_Player::GetInputDataAsset() const
{
	return InputDataAsset;
}


void AICC_Player::SetInputModeToGameOnly()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void AICC_Player::SetInputModeToGameAndUI()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

FKey AICC_Player::GetInteractKey() const
{
	if (InputDataAsset)
	{
		if (UInputAction* Action = InputDataAsset->FindNativeInputByTag(Icc_InputTags::InputTag_Interact))
		{
			if (const APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				if (ULocalPlayer* LP = PC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						TArray<FKey> Keys = Sub->QueryKeysMappedToAction(Action);
						if (Keys.Num() > 0) return Keys[0];
					}
				}
			}
		}
	}
	return EKeys::E;
}
