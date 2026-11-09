// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_ChangeLevel.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Managers/SceneLoader.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UEvent_ChangeLevel::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
   if (!Player || !LevelTag.IsValid()) return;
    auto GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());

    if (bUsePlayerStart)
    {
        // parse map and spawn strings from gameplay tags for scene loader
        FString MapName = LevelTag.GetTagName().ToString();
        MapName.Split(TEXT("."), nullptr, &MapName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

        if (PlayerStartTag.IsValid())
        {
            FString SpawnPoint;
            PlayerStartTag.GetTagName().ToString().Split(TEXT("."), nullptr, &SpawnPoint, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            USceneLoader::LoadSceneWithTag(Player, FName(*MapName), SpawnPoint, false);
        }
    }
    else
    {
        //if (GI)
        //{
        //    
        //    // sanitize map name and store transform to return here after battle/event
        //    FString CleanMapName = GI->GetWorld()->GetMapName();
        //    if (CleanMapName.IsEmpty()) return;
        //    CleanMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

        //    GI->SetLastMainMapName(FName(*CleanMapName));
        //    GI->SavePlayerTransformBegin(Player, true); 
        //    GI->StoreLastPlayerTransform(Player, Player->GetActorLocation(), Player->GetActorRotation());
        //}
        //UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelTag.GetTagName().ToString())));

        UWorld* World = Player->GetWorld();
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("Event_ChangeLevel: Player %s has no World."), *Player->GetName());
            return;
        }

        if (GI)
        {
            // in PIE the map name carries the "UEDPIE_0_" prefix, strip it or the return trip won't find the map
            FString CleanMapName = World->GetMapName();
            CleanMapName.RemoveFromStart(World->StreamingLevelsPrefix);

            GI->SetLastMainMapName(FName(*CleanMapName));

            // store first, then promote: the reverse order would save the previous position
            GI->StoreLastPlayerTransform(Player, Player->GetActorLocation(), Player->GetActorRotation());
            GI->SavePlayerTransformBegin(Player, true);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Event_ChangeLevel: GameInstance is not UICantCryGameInstance, return point not saved."));
        }

        // temporary convention until the registry exists: last tag segment == .umap name (Maps.City -> City.umap)
        FString MapName;
        if (!LevelTag.GetTagName().ToString().Split(TEXT("."), nullptr, &MapName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
        {
            MapName = LevelTag.GetTagName().ToString();
        }

        UE_LOG(LogTemp, Log, TEXT("Event_ChangeLevel: opening map '%s' from tag %s."), *MapName, *LevelTag.ToString());
        UGameplayStatics::OpenLevel(World, FName(*MapName));




    }
}