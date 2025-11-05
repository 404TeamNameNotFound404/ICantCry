#include "ICantCryGameInstance.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Kismet/GameplayStatics.h"

void UICantCryGameInstance::Init()
{
	Super::Init();

	ActiveBestiaryUI = nullptr;
	
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


void UICantCryGameInstance::UpdateBestiaryUI()
{
    if (ActiveBestiaryUI && IsBestiaryUIActive())
    {
        ActiveBestiaryUI->RefreshNotesFromGameInstance();
        ActiveBestiaryUI->RefreshEmotionButtons();
        UE_LOG(LogTemp, Warning, TEXT("[GAMEINSTANCE] BestiaryUI updated"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[GAMEINSTANCE] BestiaryUI is not active, skipping update"));
    }
}

bool UICantCryGameInstance::IsBestiaryUIActive() const
{
    return ActiveBestiaryUI != nullptr && ActiveBestiaryUI->IsInViewport();
}


//{DEBUG}

void UICantCryGameInstance::DebugUnlockAll()
{
    // Sblocca tutte le emotion
    UnlockedEmotions.Empty();
    UnlockedEmotions.Add(EEmotionType::Anger);
    UnlockedEmotions.Add(EEmotionType::Fear);
    UnlockedEmotions.Add(EEmotionType::Sadness);
    UnlockedEmotions.Add(EEmotionType::Joy);
    UnlockedEmotions.Add(EEmotionType::Disgust);
    UnlockedEmotions.Add(EEmotionType::Anxiety);
    UnlockedEmotions.Add(EEmotionType::Calm);
    UnlockedEmotions.Add(EEmotionType::Jealousy);
    UnlockedEmotions.Add(EEmotionType::Shame);

    // Sblocca alcune note di test
    CollectedNotes.Empty();
    CollectedNotes.Add("Anger_Note");
    CollectedNotes.Add("Fear_Note");
    CollectedNotes.Add("Joy_Note");

    UpdateBestiaryUI();
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked all emotions and test notes"));
}

void UICantCryGameInstance::DebugResetBestiary()
{
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Bestiary reset"));
    UnlockedEmotions.Empty();
    CollectedNotes.Empty();
    UpdateBestiaryUI();
    
}

void UICantCryGameInstance::DebugPrintBestiaryStatus()
{
   UE_LOG(LogTemp, Warning, TEXT("=== BESTIARY STATUS ==="));
    UE_LOG(LogTemp, Warning, TEXT("Emotions: %d"), UnlockedEmotions.Num());
    for (EEmotionType Emotion : UnlockedEmotions)
    {
        UE_LOG(LogTemp, Warning, TEXT("  - %s"), *UEnum::GetValueAsString(Emotion));
    }
    UE_LOG(LogTemp, Warning, TEXT("Notes: %d"), CollectedNotes.Num());
    for (const FString& Note : CollectedNotes)
    {
        UE_LOG(LogTemp, Warning, TEXT("  - %s"), *Note);
    }
    UE_LOG(LogTemp, Warning, TEXT("======================="));
}


void UICantCryGameInstance::DebugSimulateMobDeath(FString MobType)
{
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Simulating death of mob type: %s"), *MobType);
    
    // Simula lo sblocco basato sul tipo di mob
    if (MobType.Equals("Anger", ESearchCase::IgnoreCase))
    {
        UnlockedEmotions.Add(EEmotionType::Anger);
        CollectedNotes.Add("Anger_Note");
    }
    else if (MobType.Equals("Fear", ESearchCase::IgnoreCase))
    {
        UnlockedEmotions.Add(EEmotionType::Fear);
        CollectedNotes.Add("Fear_Note");
    }
    else if (MobType.Equals("Sadness", ESearchCase::IgnoreCase))
    {
        UnlockedEmotions.Add(EEmotionType::Sadness);
        CollectedNotes.Add("Sadness_Note");
    }
    else if (MobType.Equals("Joy", ESearchCase::IgnoreCase))
    {
        UnlockedEmotions.Add(EEmotionType::Joy);
        CollectedNotes.Add("Joy_Note");
    }
    else if (MobType.Equals("Disgust", ESearchCase::IgnoreCase))
    {
        UnlockedEmotions.Add(EEmotionType::Disgust);
        CollectedNotes.Add("Disgust_Note");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unknown mob type. Use: Anger, Fear, Sadness, Joy, Disgust, etc."));
        return;
    }
    
    UpdateBestiaryUI();
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked content for %s"), *MobType);
}

void UICantCryGameInstance::DebugSimulateAllMobDeaths()
{
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Simulating death of ALL mob types"));
    
    // Sblocca tutto
    UnlockedEmotions.Add(EEmotionType::Anger);
    UnlockedEmotions.Add(EEmotionType::Fear);
    UnlockedEmotions.Add(EEmotionType::Sadness);
    UnlockedEmotions.Add(EEmotionType::Joy);
    UnlockedEmotions.Add(EEmotionType::Disgust);
    UnlockedEmotions.Add(EEmotionType::Anxiety);
    UnlockedEmotions.Add(EEmotionType::Calm);
    UnlockedEmotions.Add(EEmotionType::Jealousy);
    UnlockedEmotions.Add(EEmotionType::Shame);
    
    CollectedNotes.Add("Anger_Note");
    CollectedNotes.Add("Fear_Note");
    CollectedNotes.Add("Sadness_Note");
    CollectedNotes.Add("Joy_Note");
    CollectedNotes.Add("Disgust_Note");
    CollectedNotes.Add("Anxiety_Note");
    CollectedNotes.Add("Calm_Note");
    CollectedNotes.Add("Jealousy_Note");
    CollectedNotes.Add("Shame_Note");
    
    UpdateBestiaryUI();
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked ALL bestiary content"));
}
