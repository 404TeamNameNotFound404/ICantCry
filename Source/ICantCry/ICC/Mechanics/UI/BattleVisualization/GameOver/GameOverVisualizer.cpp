// Fill out your copyright notice in the Description page of Project Settings.
#include "GameOverVisualizer.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Managers/UtilityFunctions.h"

UButton* UGameOverVisualizer::GetButton() const
{
	return LoadPreviousSave;
}

void UGameOverVisualizer::NativeConstruct()
{
	Super::NativeConstruct();

	Retry->OnClicked.AddDynamic(this, &UGameOverVisualizer::RetryBattle);
	LoadPreviousSave->OnClicked.AddDynamic(this, &UGameOverVisualizer::LoadPrevious);
}


void UGameOverVisualizer::RetryBattle()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	// DebugHelper::AddMessageToLog("---Retry Pressed----");
	// DebugHelper::AddMessageToLog("Restarting the battle ...");
	//
	// AEnemySpawnManager* SpawnManager = Instance->GetCurrentPlayer()
	// ->GetBattleHUD()
	// ->GetBattleHandler()
	// ->GetEnemySpawnManager();
	//
	// if (!SpawnManager)
	// {
	// 	DebugHelper::LogMessage(9, FColor::Red,"SpawnManager is invalid");
	// 	return;
	// }
	//
	// for (AMob* Emotion : SpawnManager->GetMemory().EmotionsSpawned)
	// {
	// 	SpawnManager->ResetBattle(Emotion);
	// 	Emotion->ReinizializeTree();
	// }
	//
	// Instance->GetCurrentPlayer()->GetBattleHUD()->RequestBulletPreparation();
	// Instance->GetCurrentPlayer()->GetBattleHUD()->Reset(SpawnManager->GetMemory().InBattleBullets);
	//
	// Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().RejoinQueue(SpawnManager->GetMemory().EmotionsSpawned, Instance->GetCurrentPlayer());
	// Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->Reload();
	
	
	Instance->CachedBattleMemory.bBattleRetried = true;
	UtilityFunctions::LoadSceneByName(GetWorld(),Instance->LastBattleSceneLoaded);
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UGameOverVisualizer::LoadPrevious()
{
	// TODO add load save or checkpoint
}