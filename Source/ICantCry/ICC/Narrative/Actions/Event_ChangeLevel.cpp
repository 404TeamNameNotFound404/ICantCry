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
        if (GI)
        {
            // sanitize map name and store transform to return here after battle/event
            FString CleanMapName = GetWorld()->GetMapName();
            CleanMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

            GI->SetLastMainMapName(FName(*CleanMapName));
            GI->SavePlayerTransformBegin(Player, true); 
            GI->StoreLastPlayerTransform(Player, Player->GetActorLocation(), Player->GetActorRotation());
        }
        UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelTag.GetTagName().ToString())));
    }
}