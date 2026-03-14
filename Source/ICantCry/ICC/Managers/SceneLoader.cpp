// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneLoader.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"


// void USceneLoader::LoadSceneByName(UObject *WorldContextObject, const FName &SceneName)
// {
//     if (!WorldContextObject) return;
//     UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
// }

// void USceneLoader::LoadSceneByName(UObject* WorldContextObject, const FName& SceneName, bool bRecreatePlayer)
// {
// 	if (!WorldContextObject) return;

// 	if (bRecreatePlayer)
// 	{
// 		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
// 		if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance()))
// 		{
// 			Instance->SetCanRecreatePlayer(true); //
// 		}
// 	}
// 	UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
// }

// void USceneLoader::LoadSceneWithTag(UObject* WorldContextObject, const FName& SceneName, const FString& PlayerStartTag, bool bRecreatePlayer)
// {
//     if (!WorldContextObject) return;

//     // Attiviamo la ricreazione del player nel GameInstance per il nuovo livello
//     if (bRecreatePlayer)
//     {
//         UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
//         if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance()))
//         {
//             Instance->SetCanRecreatePlayer(true);
//         }
//     }

//     // Costruiamo la stringa opzioni per ChoosePlayerStart del GameMode
//     FString Options = "";
//     if (!PlayerStartTag.IsEmpty())
//     {
//         Options = FString::Printf(TEXT("?PlayerStart=%s"), *PlayerStartTag);
//     }

//     DebugHelper::LogSuccess(FString::Printf(TEXT("Loading Level: %s | Tag: %s"), *SceneName.ToString(), *PlayerStartTag));

//     // Carichiamo il livello passando le Options
//     UGameplayStatics::OpenLevel(WorldContextObject, SceneName, true, Options);
// }


void USceneLoader::LoadSceneByName(UObject *WorldContextObject, const FName &SceneName)
{
    if (!WorldContextObject) return;
    UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
}

void USceneLoader::LoadSceneByName(UObject* WorldContextObject, const FName& SceneName, bool bRecreatePlayer)
{
	if (!WorldContextObject) return;

	if (bRecreatePlayer)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance()))
		{
			Instance->SetCanRecreatePlayer(true);
		}
	}
	UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
}

void USceneLoader::LoadSceneWithTag(UObject* WorldContextObject, const FName& SceneName, const FString& PlayerStartTag, bool bRecreatePlayer)
{
    if (!WorldContextObject) return;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance()))
    {
        Instance->SetCanRecreatePlayer(bRecreatePlayer);
    }

    FString Options = "";
    if (!PlayerStartTag.IsEmpty())
    {
        Options = FString::Printf(TEXT("?PlayerStart=%s"), *PlayerStartTag);
    }

    UGameplayStatics::OpenLevel(WorldContextObject, SceneName, true, Options);
}
