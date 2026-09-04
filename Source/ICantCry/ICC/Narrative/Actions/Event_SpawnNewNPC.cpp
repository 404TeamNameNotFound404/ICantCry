// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_SpawnNewNPC.h"
#include "Engine/World.h"

#include "ICantCry/ICC/Narrative/Core/CinematicManager.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UEvent_SpawnNewNPC::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{

    UWorld* World = Player ? Player->GetWorld() : (Context ? Context->GetWorld() : nullptr);


    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_SpawnNewNPC: World null. NPC did not spawn."));
        return;
    }

    UCinematicManager* CinematicSubsystem = World->GetSubsystem<UCinematicManager>();
    if (!CinematicSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_SpawnNewNPC: CinematicManager not found in the World"));
        return;
    }

    CinematicSubsystem->FadeAndSpawnNewNPC(NPCClassToSpawn, DestinationPointTag, FadeDuration, BlackScreenHoldTime);
}