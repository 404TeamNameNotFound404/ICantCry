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