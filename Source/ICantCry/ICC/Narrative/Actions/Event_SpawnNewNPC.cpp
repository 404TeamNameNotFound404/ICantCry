// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_SpawnNewNPC.h"
#include "Engine/World.h"

#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/Core/CinematicManager.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UEvent_SpawnNewNPC::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (!Context || !Context->GetWorld()) return;

    UCinematicManager* CinematicSubsystem = Context->GetWorld()->GetSubsystem<UCinematicManager>();
    if (CinematicSubsystem)
    {
        CinematicSubsystem->FadeAndSpawnNewNPC(NPCClassToSpawn, DestinationPointTag, FadeDuration, BlackScreenHoldTime);
    }
}