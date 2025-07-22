// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "SceneLoader.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API USceneLoader : public UObject
{
	GENERATED_BODY()

public:

    // to load a level 
    static void LoadSceneByName(UObject* WorldContextObject, const FName& SceneName);
	
};
