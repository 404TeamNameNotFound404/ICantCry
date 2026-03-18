// Fill out your copyright notice in the Description page of Project Settings.


#include "EncounterManager.h"

#include "SceneLoader.h"
#include "UtilityFunctions.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

UEncounterManager::UEncounterManager()
{
}

void UEncounterManager::Initialize(UWorld* World)
{
	
	EncounterThreshold = FMath::RandRange(50, 200);
	CurrentThreshold = EncounterThreshold;
	SprintMultiplier = 1.5f;
	LocationMultiplier = 1.0f;

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	checkf(Instance, TEXT("Instance invalid"))

	FTimerHandle TimerHandle;

	World->GetTimerManager().SetTimer(TimerHandle, [this, World, Instance]()
	{
		PlayerRef = Instance->GetCurrentPlayer();
	}, 0.25f, false);
}

void UEncounterManager::UpdateThreshold(UWorld* World)
{
	FTimerHandle TimerHandle;

	World->GetTimerManager().SetTimer(TimerHandle, [this, World]()
	{
		if (!PlayerRef.Get())
		{
			return;
		}
		
		int32 CurrentSteps = PlayerRef.Get()->GetStepCounter();
		int32 StepsTaken = CurrentSteps - LastStepCounter;

		if (StepsTaken <= 0)
		{
			return;
		}

		LastStepCounter = CurrentSteps;

		float ReductionValue = StepsTaken * LocationMultiplier;

		if (PlayerRef.Get()->IsSprinting())
		{
			ReductionValue *= SprintMultiplier;
		}

		CurrentThreshold -= ReductionValue;

		DebugHelper::LogWarning("CurrentThreshold " + FString::SanitizeFloat(CurrentThreshold));

		if (CurrentThreshold <= 0)
		{
			StartBattle(World);
			Reset();
		}
	}, 0.45f, true);
}

void UEncounterManager::SetPlayerLocationMultiplier(EPlayerLocation NewLocation)
{
	switch (NewLocation)
	{
	case EPlayerLocation::Home:
		LocationMultiplier = 0.1f;
		break;
	case EPlayerLocation::School:
		LocationMultiplier = 2.5f;
		break;
	default:
		LocationMultiplier = 1.0f; // default
		break;
	}
}


void UEncounterManager::StartBattle(UWorld* World)
{
	DebugHelper::LogSuccess("StartBattle");
	PlayerRef.Get()->ResetStepCounter();
	// UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	// Instance->StoreLastPlayerTransform(PlayerRef.Get(), PlayerRef.Get()->GetActorLocation(), PlayerRef.Get()->GetActorRotation());
	// Instance->SavePlayerTransformBegin(Instance->GetCurrentPlayer(), true);
	// UtilityFunctions::LoadSceneByName(World, "RandomSpawner");

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
    if (Instance)
    {

		//RECUPERO E PULIZIA DEL NOME MAPPA
        FString CleanMapName = World->GetMapName();

		// Rimuove i prefissi del Play-In-Editor (es. UEDPIE_0_)
        CleanMapName.RemoveFromStart(World->StreamingLevelsPrefix);

        // Usa la funzione Setter
        Instance->SetLastMainMapName(FName(*CleanMapName));
        
        Instance->SavePlayerTransformBegin(Instance->GetCurrentPlayer(), true);
        Instance->StoreLastPlayerTransform(Instance->GetCurrentPlayer(), Instance->GetCurrentPlayer()->GetActorLocation(), Instance->GetCurrentPlayer()->GetActorRotation());
    }

    UtilityFunctions::LoadSceneByName(World, "RandomSpawner");
}

void UEncounterManager::Reset()
{
	EncounterThreshold = FMath::RandRange(50, 200);
	CurrentThreshold = EncounterThreshold;
	LastStepCounter = PlayerRef.Get() ? PlayerRef.Get()->GetStepCounter() : 0; 
}
