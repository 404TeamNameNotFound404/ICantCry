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

	AICC_Player* Player = GetWorld()->SpawnActor<AICC_Player>(PlayerBp, PersistentData->PlayerPosition, PersistentData->PlayerOrientation, SpawnParams);

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

void UICantCryGameInstance::SavePlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const
{
	PersistentData->PlayerPosition = LastPosition;
	PersistentData->PlayerOrientation = LastOrientation;

	DebugHelper::LogSuccess("New Saved Position " + PersistentData->PlayerPosition.ToString());
	DebugHelper::LogSuccess("New Saved Orientation " + PersistentData->PlayerOrientation.ToString());
}

void UICantCryGameInstance::LoadLastPlayerTransform()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	checkf(PlayerController, TEXT("PlayerController is NULL at UICantCryGameInstance::LoadLastPlayerTransform"));
	AICC_Player* Player = Cast<AICC_Player>(PlayerController->GetPawn());
	checkf(Player, TEXT("Player cast is null at UICantCryGameInstance::LoadLastPlayerTransform"));

	Player->SetActorLocation(PersistentData->PlayerPosition);
	Player->SetActorRotation(PersistentData->PlayerOrientation);
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
