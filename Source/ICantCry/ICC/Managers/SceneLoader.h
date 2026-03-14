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

    // // to load a level 
    // static void LoadSceneByName(UObject* WorldContextObject, const FName& SceneName);

    // // Caricamento con opzione ricreazione player (usato dal BattleHandler)
	// static void LoadSceneByName(UObject* WorldContextObject, const FName& SceneName, bool bRecreatePlayer);
	
    // /** * NUOVA: Carica una scena specificando un tag di spawn.
    //  * Gestisce internamente la comunicazione con il GameInstance e il GameMode.
    //  */
    // static void LoadSceneWithTag(UObject* WorldContextObject, const FName& SceneName, const FString& PlayerStartTag, bool bRecreatePlayer = true);



    // Caricamento semplice
    static void LoadSceneByName(UObject* WorldContextObject, const FName& SceneName);
    
    // Caricamento con flag ricreazione (True = usa coordinate salvate nel GameInstance)
	static void LoadSceneByName(UObject* WorldContextObject, const FName& SceneName, bool bRecreatePlayer);

    // Caricamento con tag specifico (bRecreatePlayer deve essere False per usare il PlayerStart)
    static void LoadSceneWithTag(UObject* WorldContextObject, const FName& SceneName, const FString& PlayerStartTag, bool bRecreatePlayer);

};
