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

void UICantCryGameInstance::RecreatePlayer(UWorld* World,FVector& PreviousPosition, FRotator& PreviousRotation, float& CurrentHp,
	float& CurrentAp) const
{
	PreviousPosition = PersistentData->PlayerPosition;
	PreviousRotation = PersistentData->PlayerOrientation;
	CurrentHp = PersistentData->PlayerHp;
	//CurrentAp = PersistentData->AbilityPoints;
	
	checkf(World, TEXT("World is null in UICantCryGameInstance::RecreatePlayer "));

	AICC_Player* Player = World->SpawnActor<AICC_Player>(AICC_Player::StaticClass(), PreviousPosition, PreviousRotation);

	checkf(Player, TEXT("Player is null in UICantCryGameInstance::RecreatePlayer"));

	APlayerController* PlayerController = Player->GetController<APlayerController>();
	
	checkf(PlayerController, TEXT("Player controller is null in UICantCryGameInstance::RecreatePlayer"));

	PlayerController->Possess(Player);
	
	DebugHelper::LogSuccess("Player recreated successfully");
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
