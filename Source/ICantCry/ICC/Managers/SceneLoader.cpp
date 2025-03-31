// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneLoader.h"

// Sets default values
ASceneLoader::ASceneLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;  
	PrimaryActorTick.bStartWithTickEnabled= false;
}

void ASceneLoader::LoadScene(FName SceneName)
{
	if(AvailableScenes.Contains(SceneName))
	{
		// call UDontDestroyOnLoad to save data 
		UGameplayStatics::OpenLevel(this, SceneName);

		// UE doesn't have a direct event to know when the level is fully loaded. So i try use the timer
		// hatgpt says you can also use UGameInstance's OnPostLoadMap() method
		// GetWorld()->GetTimerManager().SetTimer(TimerHandle_LoadScene, this, &ASceneLoader::OnSceneLoaded, 1.0f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Scene %s not found in AvailableScenes array!"), *SceneName.ToString());
	}
}

// void ASceneLoader::OnSceneLoaded()
// {
//    	UICantCryGameInstance* GameInstance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance());  
// 	checkf(GameInstance, TEXT("Instance is null")); 
// 	GameInstance->RecreatePlayer(GameInstance->PersistentData->PlayerPosition, GameInstance->PersistentData->PlayerOrientation);
// 	UE_LOG(LogTemp, Log, TEXT("Scene loaded, persistent data restored."));
// }

// Called when the game starts or when spawned
void ASceneLoader::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ASceneLoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

