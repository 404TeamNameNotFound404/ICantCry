// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_HideNPC.h"
#include "Engine/World.h"

#include "ICantCry/ICC/Narrative/Core/CinematicManager.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UEvent_HideNPC::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{

    UWorld* World = Player ? Player->GetWorld() : (Context ? Context->GetWorld() : nullptr);

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_HideNPC: World null ( Player & Context are not valid). No hide NPC ."));
        return;
    }

    UCinematicManager* CinematicSubsystem = World->GetSubsystem<UCinematicManager>();
    if (!CinematicSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_HideNPC: CinematicManager not found in the World."));
        return;
    }

    CinematicSubsystem->FadeAndHideNPC(TargetNPCActorTag, FadeDuration, BlackScreenHoldTime);


}