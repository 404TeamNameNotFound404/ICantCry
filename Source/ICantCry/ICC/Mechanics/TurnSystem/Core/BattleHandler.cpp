// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleHandler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "EngineUtils.h"


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

