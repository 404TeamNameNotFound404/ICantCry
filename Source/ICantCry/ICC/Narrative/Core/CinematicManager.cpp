// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"

#include "ICantCry/ICC/Actors/Player/Camera/WorldCamera.h"
#include "ICantCry/ICC/Actors/Player/Camera/CameraWaypoint.h"

void UCinematicManager::BlockPlayerInput(bool bBlock)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetIgnoreMoveInput(bBlock);
        PC->SetIgnoreLookInput(bBlock);
        if (bBlock)
        {
            PC->FlushPressedKeys();
        }
    }
}

void UCinematicManager::PerformCameraFade(float FromAlpha, float ToAlpha, float Duration)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, Duration, FLinearColor::Black, false, true);
    }
}




void UCinematicManager::FadeAndHideNPC(FName TargetNPCTag, float FadeDuration, float HoldTime)
{
    if (!GetWorld()) return;

    BlockPlayerInput(true);
    PerformCameraFade(0.0f, 1.0f, FadeDuration); // Fade Out

    // Timer per eseguire l'azione quando lo schermo è completamente nero
    FTimerHandle ActionTimer;
    GetWorld()->GetTimerManager().SetTimer(ActionTimer, [this, TargetNPCTag, HoldTime, FadeDuration]()
        {
            // 1. Nascondi l'NPC
            TArray<AActor*> FoundNPCs;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetNPCTag, FoundNPCs);
            for (AActor* NPC : FoundNPCs)
            {
                NPC->SetActorHiddenInGame(true);
                NPC->SetActorEnableCollision(false);
                NPC->SetActorTickEnabled(false);
            }

            // 2. Dopo la pausa (HoldTime), avvia il Fade In
            FTimerHandle FadeInTimer;
            GetWorld()->GetTimerManager().SetTimer(FadeInTimer, [this, FadeDuration]()
                {
                    PerformCameraFade(1.0f, 0.0f, FadeDuration);

                    // 3. Ripristina l'input alla fine del Fade In
                    FTimerHandle UnlockTimer;
                    GetWorld()->GetTimerManager().SetTimer(UnlockTimer, [this]()
                        {
                            BlockPlayerInput(false);
                        }, FadeDuration, false);

                }, HoldTime, false);

        }, FadeDuration + 0.1f, false);
}

void UCinematicManager::FadeAndShowNPC(FName TargetNPCTag, FName DestinationPointTag, float FadeDuration, float HoldTime)
{
    if (!GetWorld()) return;

    BlockPlayerInput(true);
    PerformCameraFade(0.0f, 1.0f, FadeDuration);

    FTimerHandle ActionTimer;
    GetWorld()->GetTimerManager().SetTimer(ActionTimer, [this, TargetNPCTag, DestinationPointTag, HoldTime, FadeDuration]()
        {
            // 1. Trova il punto di destinazione (es. un TargetPoint o un attore vuoto usato come marker)
            FTransform SpawnTransform = FTransform::Identity;
            TArray<AActor*> Destinations;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), DestinationPointTag, Destinations);

            if (Destinations.Num() > 0 && Destinations[0])
            {
                SpawnTransform = Destinations[0]->GetActorTransform();
            }

            // 2. Trova l'NPC, spostalo e riattivalo
            TArray<AActor*> FoundNPCs;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetNPCTag, FoundNPCs);
            for (AActor* NPC : FoundNPCs)
            {
                NPC->SetActorTransform(SpawnTransform);
                NPC->SetActorHiddenInGame(false);
                NPC->SetActorEnableCollision(true);
                NPC->SetActorTickEnabled(true);
            }

            // 3. Fade In
            FTimerHandle FadeInTimer;
            GetWorld()->GetTimerManager().SetTimer(FadeInTimer, [this, FadeDuration]()
                {
                    PerformCameraFade(1.0f, 0.0f, FadeDuration);

                    // 4. Sblocca Input
                    FTimerHandle UnlockTimer;
                    GetWorld()->GetTimerManager().SetTimer(UnlockTimer, [this]()
                        {
                            BlockPlayerInput(false);
                        }, FadeDuration, false);

                }, HoldTime, false);

        }, FadeDuration + 0.1f, false);
}

void UCinematicManager::FadeAndSpawnNewNPC(TSubclassOf<AActor> NPCClassToSpawn, FName DestinationPointTag, float FadeDuration, float HoldTime)
{
    if (!GetWorld() || !NPCClassToSpawn) return;

    BlockPlayerInput(true);
    PerformCameraFade(0.0f, 1.0f, FadeDuration); // Fade Out

    FTimerHandle ActionTimer;
    GetWorld()->GetTimerManager().SetTimer(ActionTimer, [this, NPCClassToSpawn, DestinationPointTag, HoldTime, FadeDuration]()
        {
            // 1. Trova il punto di destinazione tramite Tag
            FTransform SpawnTransform = FTransform::Identity;
            TArray<AActor*> Destinations;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), DestinationPointTag, Destinations);

            if (Destinations.Num() > 0 && Destinations[0])
            {
                SpawnTransform = Destinations[0]->GetActorTransform();
            }

            // 2. Spawna fisicamente il nuovo NPC nel livello
            GetWorld()->SpawnActor<AActor>(NPCClassToSpawn, SpawnTransform);

            // 3. Dopo la pausa (HoldTime), avvia il Fade In
            FTimerHandle FadeInTimer;
            GetWorld()->GetTimerManager().SetTimer(FadeInTimer, [this, FadeDuration]()
                {
                    PerformCameraFade(1.0f, 0.0f, FadeDuration);

                    // 4. Sblocca l'input del giocatore
                    FTimerHandle UnlockTimer;
                    GetWorld()->GetTimerManager().SetTimer(UnlockTimer, [this]()
                        {
                            BlockPlayerInput(false);
                        }, FadeDuration, false);

                }, HoldTime, false);

        }, FadeDuration + 0.1f, false);
}
