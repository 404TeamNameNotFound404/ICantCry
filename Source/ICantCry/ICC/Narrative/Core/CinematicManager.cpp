// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/UI/BarkWidget.h"




void UCinematicManager::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearAllTimersForObject(this);
    }

    BlockPlayerInput(false);
    bIsCinematicPlaying = false;
    PendingBlackScreenAction.Reset();

    Super::Deinitialize();
}


void UCinematicManager::FadeAndHideNPC(FName TargetNPCTag, float FadeDuration, float HoldTime)
{
    PlayFadeSequence([this, TargetNPCTag]()
        {
            TArray<AActor*> FoundNPCs;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetNPCTag, FoundNPCs);

            if (FoundNPCs.Num() == 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("CinematicManager::FadeAndHideNPC - nessun actor con tag '%s'"), *TargetNPCTag.ToString());
            }

            for (AActor* NPC : FoundNPCs)
            {
                if (!NPC) continue;
                NPC->SetActorHiddenInGame(true);
                NPC->SetActorEnableCollision(false);
                NPC->SetActorTickEnabled(false);
            }
        }, FadeDuration, HoldTime);
}


void UCinematicManager::FadeAndShowNPC(FName TargetNPCTag, FName DestinationPointTag, float FadeDuration, float HoldTime)
{
    PlayFadeSequence([this, TargetNPCTag, DestinationPointTag]()
        {
            FTransform Destination;
            const bool bHasDestination = FindDestination(DestinationPointTag, Destination);

            TArray<AActor*> FoundNPCs;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetNPCTag, FoundNPCs);

            if (FoundNPCs.Num() == 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("CinematicManager::FadeAndShowNPC - nessun actor con tag '%s'"), *TargetNPCTag.ToString());
            }

            for (AActor* NPC : FoundNPCs)
            {
                if (!NPC) continue;

                // without a valid destination the npc stays where it is, before it was sent to the world origin
                if (bHasDestination)
                {
                    NPC->SetActorTransform(Destination);
                }
                NPC->SetActorHiddenInGame(false);
                NPC->SetActorEnableCollision(true);
                NPC->SetActorTickEnabled(true);
            }
        }, FadeDuration, HoldTime);
}


void UCinematicManager::FadeAndSpawnNewNPC(TSubclassOf<AActor> NPCClassToSpawn, FName DestinationPointTag, float FadeDuration, float HoldTime)
{
    if (!NPCClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("CinematicManager::FadeAndSpawnNewNPC - NPCClassToSpawn non impostata"));
        return;
    }

    PlayFadeSequence([this, NPCClassToSpawn, DestinationPointTag]()
        {
            FTransform Destination;
            if (!FindDestination(DestinationPointTag, Destination))
            {
                // spawning at the world origin would put the npc inside walls or under the map
                UE_LOG(LogTemp, Error, TEXT("CinematicManager::FadeAndSpawnNewNPC - spawn di %s annullato, destinazione mancante"), *NPCClassToSpawn->GetName());
                return;
            }

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            if (!GetWorld()->SpawnActor<AActor>(NPCClassToSpawn, Destination, Params))
            {
                UE_LOG(LogTemp, Error, TEXT("CinematicManager::FadeAndSpawnNewNPC - SpawnActor fallito per %s"), *NPCClassToSpawn->GetName());
            }
        }, FadeDuration, HoldTime);
}


void UCinematicManager::PlayFadeSequence(TFunction<void()> ActionAtBlack, float FadeDuration, float HoldTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("CinematicManager::PlayFadeSequence - World nullo"));
        return;
    }

    // a second transition would overwrite the timers of the first one and its unlock would never run:
    // apply the change right away instead, the running fade already covers the screen
    if (bIsCinematicPlaying)
    {
        UE_LOG(LogTemp, Warning, TEXT("CinematicManager - transizione gia' in corso, la nuova azione viene eseguita subito senza fade"));
        if (ActionAtBlack) ActionAtBlack();
        return;
    }

    bIsCinematicPlaying = true;
    CurrentFadeDuration = FMath::Max(FadeDuration, MinStepDuration);
    CurrentHoldTime = FMath::Max(HoldTime, MinStepDuration);
    PendingBlackScreenAction = MoveTemp(ActionAtBlack);

    BlockPlayerInput(true);
    PerformCameraFade(0.0f, 1.0f, CurrentFadeDuration);

    // weak lambda: if the subsystem dies with the world the timer simply does nothing
    World->GetTimerManager().SetTimer(BlackScreenTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]() { OnScreenBlack(); }),
        CurrentFadeDuration + 0.1f, false);
}


void UCinematicManager::OnScreenBlack()
{
    if (PendingBlackScreenAction)
    {
        PendingBlackScreenAction();
        PendingBlackScreenAction.Reset();
    }

    GetWorld()->GetTimerManager().SetTimer(FadeInTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]() { OnStartFadeIn(); }),
        CurrentHoldTime, false);
}


void UCinematicManager::OnStartFadeIn()
{
    PerformCameraFade(1.0f, 0.0f, CurrentFadeDuration);

    GetWorld()->GetTimerManager().SetTimer(FinishTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]() { FinishCinematic(); }),
        CurrentFadeDuration, false);
}


void UCinematicManager::FinishCinematic()
{
    BlockPlayerInput(false);
    bIsCinematicPlaying = false;

    // a dialogue or a bark opened during the fade owns the input: it will restore it when it closes
    TArray<UUserWidget*> OpenDialogues;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), OpenDialogues, UDialogueWidget::StaticClass(), true);
    TArray<UUserWidget*> OpenBarks;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), OpenBarks, UBarkWidget::StaticClass(), true);

    if (OpenDialogues.Num() > 0 || OpenBarks.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("CinematicManager - fine transizione, dialogo/bark aperto: input lasciato alla UI"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("CinematicManager::FinishCinematic - PlayerController non trovato, impossibile ripristinare l'input"));
        return;
    }

    // the dialogue may have closed during the fade: give the controls back to the map explicitly
    PC->SetInputMode(FInputModeGameOnly());
    PC->bShowMouseCursor = false;

    if (AICC_Player* Player = Cast<AICC_Player>(PC->GetPawn()))
    {
        Player->SetDialogueMovementLock(false);
    }

    UE_LOG(LogTemp, Log, TEXT("CinematicManager - fine transizione, input di gioco ripristinato (MoveIgnored=%d, LookIgnored=%d)"),
        PC->IsMoveInputIgnored(), PC->IsLookInputIgnored());
}


void UCinematicManager::BlockPlayerInput(bool bBlock)
{
    // SetIgnoreMoveInput is a counter in the engine (+1 / -1): calling it twice with true
    // and once with false leaves the player locked, so this manager changes it only once per state
    if (bBlock == bInputBlockedByCinematic) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("CinematicManager::BlockPlayerInput - PlayerController non trovato"));
        return;
    }

    PC->SetIgnoreMoveInput(bBlock);
    PC->SetIgnoreLookInput(bBlock);
    if (bBlock)
    {
        PC->FlushPressedKeys();
    }

    bInputBlockedByCinematic = bBlock;
}


void UCinematicManager::PerformCameraFade(float FromAlpha, float ToAlpha, float Duration)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !PC->PlayerCameraManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("CinematicManager::PerformCameraFade - PlayerCameraManager non trovato"));
        return;
    }

    PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, Duration, FLinearColor::Black, false, true);
}


bool UCinematicManager::FindDestination(FName DestinationPointTag, FTransform& OutTransform) const
{
    TArray<AActor*> Destinations;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DestinationPointTag, Destinations);

    if (Destinations.Num() == 0 || !Destinations[0])
    {
        UE_LOG(LogTemp, Warning, TEXT("CinematicManager - nessun actor con tag destinazione '%s'"), *DestinationPointTag.ToString());
        return false;
    }

    OutTransform = Destinations[0]->GetActorTransform();
    return true;
}
