// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleHandler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"


// Sets default values
ABattleHandler::ABattleHandler(): TurnBasedSystem(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ABattleHandler::BeginPlay()
{
	Super::BeginPlay();
	
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	for (TActorIterator<AEnemySpawnManager> It(GetWorld()); It; ++It)
	{
		SpawnManager = *It;
		break;
	}
	
	TurnBasedSystem = NewObject<UTurnBasedSystem>();
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), BattleInfoWidget);
	BattleInfo = Cast<UBattleInfo>(Widget);
	BattleInfo->AddToViewport();
	TurnBasedSystem->Start2(GetWorld(), &SpawnManager->GetMemory());
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	checkf(PlayerController, TEXT("PlayerController is null at ABattleHandler::BeginPlay"));

	PC = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

}

// Called every frame
void ABattleHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnBasedSystem->Update(GetWorld(), &SpawnManager->GetMemory());

	if (DebugHelper::IsGamepadPlugged())
	{
		PC->bShowMouseCursor = false;
		bControllerPlugged = true;
	}
	else
	{
		PC->bShowMouseCursor = true;
		bControllerPlugged = false;
	}
}

UTurnBasedSystem *ABattleHandler::GetTurnBasedSystem() const
{
    return TurnBasedSystem;
}

UBattleInfo* ABattleHandler::GetBattleInfo() const
{
	checkf(BattleInfo, TEXT("Battle Info is invalid"))
	return BattleInfo;
}

AEnemySpawnManager* ABattleHandler::GetEnemySpawnManager()
{
	return SpawnManager;
}

bool ABattleHandler::IsControllerPlugged() const
{
	return bControllerPlugged;
}

void ABattleHandler::Fire(const FVector& DeltaLocation ,const FLinearColor& Color)
{
	const FVector SpawnLocation = Instance->GetCurrentPlayer()->GetActorLocation();
	const FVector Direction = (DeltaLocation - SpawnLocation).GetSafeNormal();
	const FRotator BulletRotation = Direction.Rotation();
	StartVfxShootLocation = SpawnLocation + FVector{100, 0,0}; // Todo adapt offset according to the muzzle position
	
	Flash = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		MuzzleFlash,
		StartVfxShootLocation,
		BulletRotation,
		FVector{50,50,50},
		false
	);
	
	Flash->SetVariablePosition("User.TargetPosition", StartVfxShootLocation);
	
	Flash->Activate(true);
	Flash->SetVariableLinearColor(FName("User.Color"), Color);
	BeamPosition = StartVfxShootLocation;
	
	// Flash->SetWorldScale3D(FVector(10.0f, 10.0f, 10.0f));
	
	DebugHelper::LogMessage(10, FColor::White,"Vfx fire");
	
	bMovingMuzzle = true;

	GetWorld()->GetTimerManager().SetTimer(BeamTimer, [this, DeltaLocation]
	{
		UpdateMuzzleFlashPosition(DeltaLocation);
	}, 0.01f, true);
	
	// GetWorld()->GetTimerManager().SetTimer(BeamTimer,  [&]
	// {
	// 	UpdateMuzzleFlashPosition(DeltaLocation);
	// }, 0.01f, true);
}

void ABattleHandler::UpdateMuzzleFlashPosition(const FVector& Location)
{
	if (!Flash)
	{
		DebugHelper::LogError("Flash null");
		return;
	}
	
	if (!bMovingMuzzle)
	{
		DebugHelper::LogError("Moving muzzle false");
		return;
	} 
	
	constexpr float MovementSpeed = 3000.f; 
    
	const FVector NewPos = FMath::VInterpConstantTo(BeamPosition, Location, GetWorld()->GetDeltaSeconds(), MovementSpeed);
	BeamPosition = NewPos;
	
	Flash->SetVariablePosition(FName("User.TargetPosition"), BeamPosition);
	
	Flash->SetWorldLocation(BeamPosition);

	// if (FVector::DistSquared(BeamPosition,Location) < 100.f)
	if (FVector::DistSquared(BeamPosition, Location) <= FMath::Square(10.f))
	{
		bMovingMuzzle = false;
		Flash->Deactivate();
	}
}

