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
	
	Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	
	World->GetTimerManager().SetTimer(EncounterTimer, this, &UEncounterManager::TickEncouter, EncounterRateSpeed, true);
	
	BattleScenes = {
		"RandomSpawner",
		"RandomBattle1",
		"RandomBattle2"
	};
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
		
		const int32 CurrentSteps = PlayerRef.Get()->GetStepCounter();
		const int32 StepsTaken = CurrentSteps - LastStepCounter;

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

void UEncounterManager::SetPaused(const bool& bPaused)
{
	bEncounterTimerPaused = bPaused;
}

void UEncounterManager::TickEncouter()
{
	if (!PlayerRef.IsValid())
	{
		PlayerRef = Instance->GetCurrentPlayer();
	}
	
	if (bEncounterTimerPaused)
	{
		return;
	}

	const int32 CurrentSteps = PlayerRef.Get()->GetStepCounter();
	const int32 StepsTaken = CurrentSteps - LastStepCounter;
	
	if (StepsTaken <= 0) return;

	LastStepCounter = CurrentSteps;
	float ReductionValue = StepsTaken * LocationMultiplier;

	if (PlayerRef.Get()->IsSprinting())
	{
		ReductionValue *= SprintMultiplier;
	}

	CurrentThreshold -= ReductionValue;

	if (CurrentThreshold <= 0)
	{
		StartBattle(PlayerRef.Get()->GetWorld());
		Reset();
	}
}


void UEncounterManager::StartBattle(UWorld* World)
{
	DebugHelper::LogSuccess("StartBattle");
	PlayerRef.Get()->ResetStepCounter();
	Instance->StoreLastPlayerTransform(PlayerRef.Get(), PlayerRef.Get()->GetActorLocation(), PlayerRef.Get()->GetActorRotation());
	Instance->SavePlayerTransformBegin(Instance->GetCurrentPlayer(), true);
	UtilityFunctions::LoadBattleSceneRandom(World, BattleScenes);
}

void UEncounterManager::Reset()
{
	EncounterThreshold = FMath::RandRange(50, 200);
	CurrentThreshold = EncounterThreshold;
	LastStepCounter = PlayerRef.Get() ? PlayerRef.Get()->GetStepCounter() : 0; 
}
