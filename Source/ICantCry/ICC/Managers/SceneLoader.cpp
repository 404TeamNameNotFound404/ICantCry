// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneLoader.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"


void USceneLoader::LoadSceneByName(UObject *WorldContextObject, const FName &SceneName)
{
     if (!WorldContextObject)
    {
     	DebugHelper::LogSuccess("WorldContextObject null");
        return;
    }

    DebugHelper::LogSuccess(FString::Printf(TEXT("Loading scene: %s"), *SceneName.ToString()));

    UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
	
}

void USceneLoader::LoadSceneByName(UObject* WorldContextObject, const FName& SceneName, bool bRecreatePlayer)
{
	if (!WorldContextObject)
	{
		DebugHelper::LogSuccess("WorldContextObject null");
		return;
	}

	if (bRecreatePlayer)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		
		if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance()))
		{
			Instance->SetCanRecreatePlayer(true);
		}
	}

	DebugHelper::LogSuccess(FString::Printf(TEXT("Loading scene: %s"), *SceneName.ToString()));

	UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
}
