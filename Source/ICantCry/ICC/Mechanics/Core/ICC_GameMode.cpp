// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_GameMode.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void AICC_GameMode::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
	checkf(GameInstance, TEXT("GameInstance is null"));

	if (GameInstance->CanRecreatePlayer())
	{
		GameInstance->RecreatePlayer();
		DebugHelper::LogWarning("Recreating player");
		GameInstance->SetCanRecreatePlayer(false);
	}

	DebugHelper::LogSuccess("no need to recreate player");
}
