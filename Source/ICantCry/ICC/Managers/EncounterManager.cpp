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
	LocationMultiplier = 1.0f; // Default: no multiplier activeted

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	checkf(Instance, TEXT("Instance invalid"))

	FTimerHandle TimerHandle;

	World->GetTimerManager().SetTimer(TimerHandle, [this, World, Instance]()
	{
		PlayerRef = Instance->GetCurrentPlayer();
		DebugHelper::LogSuccess("Player ref valid!");
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
	}, 0.45f, true); // ← Must be looping!
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
	UtilityFunctions::LoadSceneByName(World, "RandomSpawner");
}

void UEncounterManager::Reset()
{
	EncounterThreshold = FMath::RandRange(50, 200);
	CurrentThreshold = EncounterThreshold; // ← Add this line
	LastStepCounter = PlayerRef.Get() ? PlayerRef.Get()->GetStepCounter() : 0; // ← Optional: reset step tracking
}
