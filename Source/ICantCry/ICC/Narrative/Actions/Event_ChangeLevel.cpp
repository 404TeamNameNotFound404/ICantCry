// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_ChangeLevel.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Managers/SceneLoader.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

// void UEvent_ChangeLevel::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
// {
//     if (!Player || !LevelTag.IsValid()) return;

//     // 1. Estraiamo il nome del livello dal Tag (es: "Livelli.Mappa_Citta" -> "Mappa_Citta")
//     FString MapName = LevelTag.GetTagName().ToString();
//     MapName.Split(TEXT("."), nullptr, &MapName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

//     // 2. Estraiamo il tag dello spawn point (es: "Spawn.PuntoA" -> "PuntoA")
//     FString SpawnPoint = "";
//     if (PlayerStartTag.IsValid())
//     {
//         PlayerStartTag.GetTagName().ToString().Split(TEXT("."), nullptr, &SpawnPoint, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
//     }

//     // 3. Eseguiamo il caricamento tramite il SceneLoader centralizzato
//     USceneLoader::LoadSceneWithTag(Player, FName(*MapName), SpawnPoint, true);
// }

void UEvent_ChangeLevel::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (!Player || !LevelTag.IsValid()) 
    {
        DebugHelper::LogError("Event_ChangeLevel: Player null or LevelTag invalid!");
        return;
    }

    // 1. Recupero il GameInstance per poter salvare la posizione
    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());

    // Estrazione pulita del nome mappa
    FString MapName = LevelTag.GetTagName().ToString();
    MapName.Split(TEXT("."), nullptr, &MapName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

    if (bUsePlayerStart)
    {
        if (!PlayerStartTag.IsValid())
        {
            DebugHelper::LogWarning("WARNING: bUsePlayerStart is enabled, but you haven't entered a PlayerStartTag! The player will spawn at a random location.");
            USceneLoader::LoadSceneWithTag(Player, FName(*MapName), "", false);
        }
        else
        {
            FString SpawnPoint;
            PlayerStartTag.GetTagName().ToString().Split(TEXT("."), nullptr, &SpawnPoint, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            
            // Usiamo il tag e disattiviamo la ricreazione per non sovrascrivere la posizione
            USceneLoader::LoadSceneWithTag(Player, FName(*MapName), SpawnPoint, false);
        }
    }
    else
    {
        // --- MODIFICA QUI: MODALITÀ BATTAGLIA/NPC ---
        
       if (GI)
        {

            // RECUPERO E PULIZIA DEL NOME MAPPA
            FString CleanMapName = GetWorld()->GetMapName();
            CleanMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

            // Salviamo il nome pulito nel GameInstance
            GI->SetLastMainMapName(FName(*CleanMapName));


            // 2. Prepara il sistema per ricreare il player al ritorno
            GI->SavePlayerTransformBegin(Player, true); 

            // 3. Salva la posizione e rotazione esatta davanti all'NPC
            GI->StoreLastPlayerTransform(Player, Player->GetActorLocation(), Player->GetActorRotation());
            
            DebugHelper::LogSuccess(FString::Printf(TEXT("Map '%s' and location saved. Battle begins!"), *GetWorld()->GetMapName()));
        }

        // 4. Carica l'arena di battaglia definita nel Tag del livello
        DebugHelper::LogSuccess("Event_ChangeLevel: Loading Arena...");
        USceneLoader::LoadSceneByName(Player, FName(*MapName), false);
    }
}