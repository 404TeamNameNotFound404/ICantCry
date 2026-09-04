// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_SpawnNPC.h"
#include "Engine/World.h"

#include "ICantCry/ICC/Narrative/Core/CinematicManager.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UEvent_SpawnNPC::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    // The World must come from something that actually lives in the level.
    // In OnDialogueEnded and in quest rewards the Context can be a DataAsset, whose GetWorld()
    // is null: that is what used to make this event silently do nothing.
    // The Player is always an Actor with a valid World, so it is the primary source.
    UWorld* World = Player ? Player->GetWorld() : (Context ? Context->GetWorld() : nullptr);

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_SpawnNPC: World null (neither Player nor Context valid). NPC not shown."));
        return;
    }

    UCinematicManager* CinematicSubsystem = World->GetSubsystem<UCinematicManager>();
    if (!CinematicSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_SpawnNPC: CinematicManager not found in the World."));
        return;
    }

    CinematicSubsystem->FadeAndShowNPC(TargetNPCActorTag, DestinationPointTag, FadeDuration, BlackScreenHoldTime);
}
