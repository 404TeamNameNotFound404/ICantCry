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
        DebugHelper::LogError("Event_ChangeLevel: Player null o LevelTag non valido!");
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
            DebugHelper::LogWarning("ATTENZIONE: bUsePlayerStart è attivo ma non hai inserito un PlayerStartTag! Il player spawnerà in un punto casuale.");
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
            // 2. SALVATAGGIO: Diciamo al GameInstance di ricordarsi DOVE siamo ora (davanti all'NPC)
            // Usiamo le stesse funzioni che usa il tuo EncounterManager
            GI->SavePlayerTransformBegin(Player->GetActorTransform());
            GI->StoreLastPlayerTransform(Player, Player->GetActorLocation(), Player->GetActorRotation());
            
            DebugHelper::LogSuccess(FString::Printf(TEXT("Posizione salvata prima della battaglia: %s"), *Player->GetActorLocation().ToString()));
        }

        // 3. CAMBIO LIVELLO: Andiamo alla mappa battaglia.
        // NOTA: Qui passiamo FALSE! 
        // Vogliamo che nella mappa battaglia il player usi il PlayerStart dell'arena,
        // non che venga ricreato sulle coordinate del mondo esterno (finirebbe nel vuoto).
        DebugHelper::LogSuccess("Event_ChangeLevel: Caricamento Arena (Posizione memorizzata per il ritorno)");
        USceneLoader::LoadSceneByName(Player, FName(*MapName), false);
    }
}