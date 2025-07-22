// Fill out your copyright notice in the Description page of Project Settings.
#include "GameOverVisualizer.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UGameOverVisualizer::NativeConstruct()
{
	Super::NativeConstruct();

	Retry->OnClicked.AddDynamic(this, &UGameOverVisualizer::RetryBattle);
	LoadPreviousSave->OnClicked.AddDynamic(this, &UGameOverVisualizer::LoadPrevious);
}


void UGameOverVisualizer::RetryBattle()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	AEnemySpawnManager* SpawnManager = Instance->GetCurrentPlayer()
	->GetBattleHUD()
	->GetBattleHandler()
	->GetEnemySpawnManager();
	
	checkf(SpawnManager, TEXT("Spawn manager invalid"))

	for (AMob* Emotion : SpawnManager->GetMemory().EmotionsSpawned)
	{
		SpawnManager->ResetBattle(Emotion);
	}

	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().RejoinQueue(SpawnManager->GetMemory().EmotionsSpawned);
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->Reload();
	
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UGameOverVisualizer::LoadPrevious()
{
	// TODO add load save or checkpoint
}


// for (AICC_Actor* Entity : Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetCopyQueue())
// {
// 	if (Entity->IsA(AICC_Player::StaticClass()))
// 	{
// 		continue;
// 	}
// 	
// 	SpawnManager->ResetBattle(Cast<AMob>(Entity));
// }

// if (Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().Queue.Num() > 0)
// {
//
// 	for (AICC_Actor* Entity : Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().Queue)
// 	{
// 		if (Entity->IsA(AICC_Player::StaticClass()))
// 		{
// 			continue;
// 		}
//
// 		// Entity->Destroy();
//
// 		SpawnManager->ResetBattle(Cast<AMob>(Entity));
// 	}
// }

// //
// FBattleMemory Memory = Instance->GetCurrentPlayer()
// 	->GetBattleHUD()
// 	->GetBattleHandler()
// 	->GetEnemySpawnManager()
// 	->GetMemory();
//
// for (int32 i = 0; i < Memory.EmotionsSpawnedClasses.Num(); ++i)
// {
// 	if (!Memory.EmotionsSpawnedClasses[i]) continue;
//
// 	const FVector& Location = Memory.Locations.IsValidIndex(i) ? Memory.Locations[i] : FVector::ZeroVector;
// 	const FRotator& Rotation = Memory.Orientations.IsValidIndex(i) ? Memory.Orientations[i] : FRotator::ZeroRotator;
//
// 	SpawnManager->RespawnEnemy(Memory.EmotionsSpawnedClasses[i], Location, Rotation);
// }
//
// Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->Reload();
// Instance->GetCurrentPlayer()->GetStats()->CurrentHealth = Instance->GetCurrentPlayer()->GetStats()->MaxHealth;
// Instance->GetCurrentPlayer()->GetBattleHUD()->ResetHealth();