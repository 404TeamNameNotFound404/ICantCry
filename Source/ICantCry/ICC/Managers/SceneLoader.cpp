// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneLoader.h"


void USceneLoader::LoadSceneByName(UObject *WorldContextObject, const FName &SceneName)
{
     if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneLoader: WorldContextObject is null."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Loading scene: %s"), *SceneName.ToString());

    UGameplayStatics::OpenLevel(WorldContextObject, SceneName);
}