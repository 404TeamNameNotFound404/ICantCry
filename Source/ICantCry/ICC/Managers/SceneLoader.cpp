// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneLoader.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"


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
