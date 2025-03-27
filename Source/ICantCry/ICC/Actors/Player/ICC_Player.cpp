// Fill out your copyright notice in the Description page of Project Settings.
#include "ICC_Player.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "ICantCry/ICC/Input/ICC_EnhancedInputCmp.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "Kismet/GameplayStatics.h"


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

	DontDestroyOnLoad->StoreBeginPlayerTransform(GetActorLocation(), GetActorRotation());

	// detect if battle handler are in the scene

	for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
	{
		MinigameHandler = *It;
		DebugHelper::LogSuccess("Minigame Handler found!");
		break;
	}
}

// Called every frame
void AICC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	LastChecked->BindNativeInputAction(InputDataAsset, Icc_InputTags::InputTag_MinigameItr, ETriggerEvent::Triggered, this, &ThisClass::Input_MinigameInteract);
}

AWorldCamera* AICC_Player::GetWorldCamera() const
{
	return WorldCamera;
}

UCameraComponent* AICC_Player::GetCamera() const
{
	return Camera;
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


void AICC_Player::Input_Interact(const FInputActionValue& InputActionValue)
{
	if (bIsInFight)
	{
		return;
	}
	
	DebugHelper::LogSuccess("Interacting with something");
}

void AICC_Player::Input_MinigameInteract(const FInputActionValue& InputActionValue)
{
	if (!MinigameHandler)
	{
		DebugHelper::LogError("Minigame is null");
		return;
	}
	MinigameHandler->StartMinigame(true);
	DebugHelper::LogSuccess("Minigame started!");
}

