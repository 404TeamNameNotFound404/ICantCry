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
	
	for (TActorIterator<AIccBattleDebugger> It(GetWorld()); It; ++It)
	{
		Debugger = *It;
		break;
	}
	
	TurnBasedSystem = NewObject<UTurnBasedSystem>();
	TurnBasedSystem->Start2(GetWorld(), &Instance->CachedBattleMemory);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	checkf(PlayerController, TEXT("PlayerController is null at ABattleHandler::BeginPlay"));

	PC = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

}

// Called every frame
void ABattleHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnBasedSystem->Update(GetWorld(), &Instance->CachedBattleMemory);

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
}

void ABattleHandler::SimulateHurt(const FLinearColor& Color)
{
	const AICC_Actor* Target = Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor();
	if (!Target) return;
	
	const FVector SpawnLocation = Target->GetActorLocation() + FVector{-50,0, 50};
	const FRotator TargetRotation = Target->GetActorRotation();
	
	UNiagaraComponent* Hurt = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HurtPrefab, SpawnLocation, TargetRotation, 
		FVector{50,50,50}, true);
	
	Hurt->Activate(true);
	Hurt->SetVariableLinearColor(FName("User.Color"), Color);
}

void ABattleHandler::SimulateAura(AICC_Actor* Target ,const float& SpawnRate ,const FLinearColor& Color)
{
	if (!Target) return;
	
	const FVector& SpawnLocation = Target->GetActorLocation() + FVector{0,0,-50};
	const FRotator& SpawnRotation = Target->GetActorRotation();
	
	Aura = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AuraPrefab, SpawnLocation, SpawnRotation, FVector{1.5f,1.5f,1.5f}, false);
	
	Aura->Activate(true);
	Aura->SetVariableLinearColor(FName("User.Color"), Color);
	Aura->SetVariableFloat(FName("User.SpawnRate"), SpawnRate);
}

void ABattleHandler::SimulateAura(AICC_Actor* Target, const float& SpawnRate, const FLinearColor& Color,
	const EBuffStatus& Status)
{
	if (!Target) return;
	
	if (Target->ActiveAuras.Contains(Status) && Target->ActiveAuras[Status])
	{
		Target->ActiveAuras[Status]->DestroyComponent();
	}
    
	const FVector& SpawnLocation = Target->GetActorLocation() + FVector{0,0,-50};
	const FRotator& SpawnRotation = Target->GetActorRotation();
    
	UNiagaraComponent* NewAura = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), 
	   AuraPrefab, SpawnLocation, SpawnRotation, FVector{1.5f}, false);
    
	if (!NewAura) return;
	
	NewAura->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    
	NewAura->Activate(true);
	NewAura->SetVariableLinearColor(FName("User.Color"), Color);
	NewAura->SetVariableFloat(FName("User.SpawnRate"), SpawnRate);
	
	Target->ActiveAuras.Add(Status, NewAura);
	
	DebugHelper::LogMessage(10, FColor::Green, 
		Target->GetActorLabel() + " has aura attached");
}

void ABattleHandler::IncreaseAura(const float& Value)
{
	if (!Aura) return;
	const float Delta = Value + AuraDecreaseValue;
	const float Result = FMath::Max(0, Delta);
	Aura->SetVariableFloat(FName("User.SpawnRate"), Result);
}

void ABattleHandler::DecreaseAura(const float& Value)
{
	if (!Aura) return;
	const float Delta = Value - AuraDecreaseValue;
	const float Result = FMath::RandRange(100.f, Delta);
	Aura->SetVariableFloat(FName("User.SpawnRate"), Result);
}

void ABattleHandler::DeactivateAura(const EBuffStatus& Status)
{
	if (!ActiveAuras.Contains(Status)) return;
	
	if (ActiveAuras[Status])
	{
		ActiveAuras[Status]->Deactivate();
	}
	
	ActiveAuras.Remove(Status);
}

void ABattleHandler::DeactivateAura()
{
	if (!Aura) return;
	Aura->Deactivate();
}

void ABattleHandler::DeactivateAura(AICC_Actor* Target, const EBuffStatus& Status)
{
	auto AuraMap = Target->ActiveAuras;
	if (!Target) return;
	if (AuraMap.IsEmpty()) return;
	
	if (AuraMap.Contains(Status))
	{
		AuraMap[Status]->Deactivate();
	}
	
	AuraMap.Remove(Status);
}

void ABattleHandler::SimulateFreezedUp(AICC_Actor* Target, const FLinearColor& Color)
{
	if (!Target) return;
	const FVector& SpawnLocation = Target->GetActorLocation() + FVector{0,0,-50};
	const FRotator& SpawnRotation = Target->GetActorRotation();
	
	Freezed = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FreezedUpPrefab, SpawnLocation, SpawnRotation,{2,2,2}, true);
	Freezed->Activate();
}

void ABattleHandler::SimulateDebuffDef(AICC_Actor* Target)
{
	if (!Target) return;
	const FVector& SpawnLocation = Target->GetActorLocation() + FVector{0,0,-50};
	const FRotator& SpawnRotation = Target->GetActorRotation();
	
	DebuffDef = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DebuffDefPrefab, SpawnLocation, SpawnRotation,{2,2,2}, true);
	DebuffDef->Activate();
}

void ABattleHandler::SimulateDebuffAtk(AICC_Actor* Target)
{
	if (!Target) return;
	const FVector& SpawnLocation = Target->GetActorLocation() + FVector{0,0,-50};
	const FRotator& SpawnRotation = Target->GetActorRotation();
	
	DebuffAtk = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DebuffAtkPrefab, SpawnLocation, SpawnRotation,{2,2,2}, true);
	DebuffAtk->Activate();
}

void ABattleHandler::DeactivateDebuffAura(const bool& InDebuffAtk)
{
	if (InDebuffAtk)
	{
		DebuffAtk->Deactivate();
	}
	
	else
	{
		DebuffDef->Deactivate();
	}
}

UBulletData* ABattleHandler::GetIndifferenceData()
{
	return Indifference;
}

void ABattleHandler::InitDebugger()
{
	if (!IsValid(Debugger)) return;
	
	Debugger->FillInitialInfo();
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
		GetWorld()->GetTimerManager().ClearTimer(BeamTimer);
	}
}

