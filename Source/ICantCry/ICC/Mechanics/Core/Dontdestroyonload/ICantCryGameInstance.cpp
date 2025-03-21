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
	PreviousPosition = PlayerPosition;
	PreviousRotation = PlayerOrientation;
	CurrentHp = PlayerHp;
	CurrentAp = AbilityPoints;

	checkf(World, TEXT("World is null in UICantCryGameInstance::RecreatePlayer "));

	AICC_Player* Player = World->SpawnActor<AICC_Player>(AICC_Player::StaticClass(), PreviousPosition, PreviousRotation);

	checkf(Player, TEXT("Player is null in UICantCryGameInstance::RecreatePlayer"));

	APlayerController* PlayerController = Player->GetController<APlayerController>();
	
	checkf(PlayerController, TEXT("Player controller is null in UICantCryGameInstance::RecreatePlayer"));

	PlayerController->Possess(Player);
	
	DebugHelper::LogSuccess("Player recreated successfully");
}

void UICantCryGameInstance::StorePlayerData(const FVector& BeginPosition, const FRotator& BeginOrientation)
{
	PlayerPosition = BeginPosition;
	PlayerOrientation = BeginOrientation;

	DebugHelper::LogSuccess("Position saved " + BeginPosition.ToString());
	DebugHelper::LogSuccess("Orientation saved " + BeginOrientation.ToString());
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
