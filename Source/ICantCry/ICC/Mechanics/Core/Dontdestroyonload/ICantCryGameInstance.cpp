#include "ICantCryGameInstance.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Kismet/GameplayStatics.h"

void UICantCryGameInstance::Init()
{
	Super::Init();
	
}

void UICantCryGameInstance::Shutdown()
{
	Super::Shutdown();
	
	//TODO SAVE GAME DATA
}

void UICantCryGameInstance::RecreatePlayer() const
{
	checkf(GetWorld(), TEXT("World is null in UICantCryGameInstance::RecreatePlayer "));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AICC_PlayerController* PlayerController = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	checkf(PlayerController, TEXT("Player controller is null in UICantCryGameInstance::RecreatePlayer"));
	
	if (APawn* ExistingPawn = PlayerController->GetPawn())
	{
		DebugHelper::LogWarning("Destroying the previous pawn");
		ExistingPawn->Destroy();
	}

	AICC_Player* Player = GetWorld()->SpawnActor<AICC_Player>(PlayerBp, PlayerRuntimeData.CurrentLocation, PlayerRuntimeData.CurrentOrientation, SpawnParams);

	checkf(Player, TEXT("Player is null in UICantCryGameInstance::RecreatePlayer"));
	
	PlayerController->Possess(Player);
	
	DebugHelper::LogSuccess("Player recreated successfully at " + PersistentData->PlayerPosition.ToString());
}

void UICantCryGameInstance::StoreBeginPlayerTransform(const FVector& BeginPosition, const FRotator& BeginOrientation) const
{
	PersistentData->PlayerPosition = BeginPosition;
	PersistentData->PlayerOrientation = BeginOrientation;

	DebugHelper::LogSuccess("Position saved " + BeginPosition.ToString());
	DebugHelper::LogSuccess("Orientation saved " + BeginOrientation.ToString());
}

void UICantCryGameInstance::StoreBeginPlayerTransform(AICC_Player* Player, const FVector BeginPosition,
	const FRotator& BeginOrientation)
{
	PlayerRuntimeData.InitialPosition = BeginPosition;
	PlayerRuntimeData.InitialOrientation = BeginOrientation;
	DebugHelper::LogSuccess("Position saved " + BeginPosition.ToString());
	DebugHelper::LogSuccess("Orientation saved " + BeginOrientation.ToString());
}

void UICantCryGameInstance::StoreLastPlayerTransform(AICC_Player* Player, const FVector& LastPosition,
	const FRotator& LastOrientation)
{
	PlayerRuntimeData.LastPositionBeforeBattle = LastPosition;
	PlayerRuntimeData.LastOrientationBeforeBattle = LastOrientation;
}

void UICantCryGameInstance::StoreLastPlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const
{
	PersistentData->PlayerPosition = LastPosition;
	PersistentData->PlayerOrientation = LastOrientation;
}

void UICantCryGameInstance::SavePlayerTransformBegin(AICC_Player* Player, const bool& PreFight)
{
	if (PreFight)
	{
		// PersistentData->PlayerPosition = Player->PlayerMemory().LastPositionBeforeBattle;
		// PersistentData->PlayerOrientation = Player->PlayerMemory().LastOrientationBeforeBattle;
		PlayerRuntimeData.CurrentLocation = PlayerRuntimeData.LastPositionBeforeBattle;
		PlayerRuntimeData.CurrentOrientation = PlayerRuntimeData.LastOrientationBeforeBattle;
		
		DebugHelper::LogMessage(10, FColor::Blue, "Position before joining the fun " + PlayerRuntimeData.LastPositionBeforeBattle.ToString());
	}
	else
	{
		PlayerRuntimeData.CurrentLocation = PlayerRuntimeData.InitialPosition;
		PlayerRuntimeData.CurrentOrientation = PlayerRuntimeData.InitialOrientation;
		DebugHelper::LogMessage(10, FColor::Cyan, "Position stored normally " + PlayerRuntimeData.InitialOrientation.ToString());
	}
}

void UICantCryGameInstance::SavePlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const
{
	PersistentData->PlayerPosition = LastPosition;
	PersistentData->PlayerOrientation = LastOrientation;

	DebugHelper::LogSuccess("New Saved Position " + PersistentData->PlayerPosition.ToString());
	DebugHelper::LogSuccess("New Saved Orientation " + PersistentData->PlayerOrientation.ToString());
}


UPlayerStats* UICantCryGameInstance::GetPlayerStats() const
{
	return PlayerStats;
}

UPersistentData* UICantCryGameInstance::GetPersistentData() const
{
	return PersistentData;
}

FDamage& UICantCryGameInstance::GetCurrentDamageData()
{
	return StoredDamage;
}

void UICantCryGameInstance::SetPlayerStats(UPlayerStats* Stats)
{
	PlayerStats = Stats;
}

void UICantCryGameInstance::SetPersistentPlayer(AICC_Player* Player)
{
	PersistentPlayer = Player;
}

AICC_Player* UICantCryGameInstance::GetCurrentPlayer() const
{
	return PersistentPlayer;
}

void UICantCryGameInstance::SetDamageData(const FDamage& Damage)
{
	StoredDamage = Damage;
	// DebugHelper::LogSuccess(FString::FromInt(Damage.BulletData->Power));
	// DebugHelper::LogSuccess(FString::FromInt(Damage.BulletData->Power));
}

void UICantCryGameInstance::SetInventory(const FInventory& Inv)
{
}

FInventory& UICantCryGameInstance::GetInventory()
{
	return Inventory;
}

bool UICantCryGameInstance::CanRecreatePlayer() const
{
	return bCanRecreatePlayer;
}

void UICantCryGameInstance::SetCanRecreatePlayer(const bool& Value)
{
	bCanRecreatePlayer = Value;
}

FPlayerMemory& UICantCryGameInstance::GetPlayerRuntimeData()
{
	return PlayerRuntimeData;
}

UWorld* UICantCryGameInstance::TryGetWorld() const
{
	UWorld* World = GetWorld();
	
	if (!World)
	{
		const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		
		if (PlayerController)
		{
			World = PlayerController->GetWorld();
		}
	}

	return World;
}
