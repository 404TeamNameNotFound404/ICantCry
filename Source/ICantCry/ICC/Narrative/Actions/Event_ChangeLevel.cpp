// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_ChangeLevel.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UEvent_ChangeLevel::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
   if (!Player || !LevelTag.IsValid()) return;

    UWorld* World = Player->GetWorld();
    if (World)
    {
        // Trasformiamo il tag "Livelli.MappaCitta" nel nome della mappa
        // Nota: Assicurati che il nome del tag finale sia identico al nome del file .umap
        FString MapName = LevelTag.GetTagName().ToString();
        
        // Se il tag è "Livelli.MappaCitta", noi vogliamo solo "MappaCitta"
        // Possiamo pulire la stringa prendendo solo l'ultima parte dopo il punto
        FString CleanMapName;
        MapName.Split(TEXT("."), nullptr, &CleanMapName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

        FString Options = "";
        if (PlayerStartTag.IsValid())
        {
            // Prendiamo il nome del tag dello spawn (es: "Spawn.Ingresso")
            FString SpawnPoint;
            PlayerStartTag.GetTagName().ToString().Split(TEXT("."), nullptr, &SpawnPoint, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            
            Options = FString::Printf(TEXT("?PlayerStart=%s"), *SpawnPoint);
        }

        UGameplayStatics::OpenLevel(World, FName(*CleanMapName), true, Options);
    }
}