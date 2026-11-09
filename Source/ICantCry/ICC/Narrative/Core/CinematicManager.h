// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinematicManager.generated.h"


class AWorldCamera;
class ACameraWaypoint;

/**
* Global manager for visual transitions and managing the presence of NPCs in the level.
*/
UCLASS()
class ICANTCRY_API UCinematicManager : public UWorldSubsystem
{
	GENERATED_BODY()



public:

    /** clears pending timers and unlocks the player if the world closes during a transition */
    virtual void Deinitialize() override;

    /** hides every actor with TargetNPCTag behind a black screen (no visibility, no collision, no tick) */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndHideNPC(FName TargetNPCTag, float FadeDuration, float HoldTime);

    /** shows the (hidden) actors with TargetNPCTag and moves them to the actor tagged DestinationPointTag */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndShowNPC(FName TargetNPCTag, FName DestinationPointTag, float FadeDuration, float HoldTime);

    /** spawns a new actor of NPCClassToSpawn at the actor tagged DestinationPointTag during the black screen */
    UFUNCTION(BlueprintCallable, Category = "ICC|Cinematics")
    void FadeAndSpawnNewNPC(TSubclassOf<AActor> NPCClassToSpawn, FName DestinationPointTag, float FadeDuration, float HoldTime);

    /** true from the start of the fade out to the end of the fade in, use it to block interactions */
    UFUNCTION(BlueprintPure, Category = "ICC|Cinematics")
    bool IsCinematicPlaying() const { return bIsCinematicPlaying; }

private:

    /** shortest duration accepted: a timer with rate 0 is never fired, which would keep the player locked forever */
    static constexpr float MinStepDuration = 0.01f;

    /** true while a transition is running */
    bool bIsCinematicPlaying = false;

    /** true only while THIS manager is ignoring move/look input, so block and unblock always come in pairs */
    bool bInputBlockedByCinematic = false;

    /** what to do while the screen is fully black (hide, move or spawn) */
    TFunction<void()> PendingBlackScreenAction;

    /** fade in duration of the running transition */
    float CurrentFadeDuration = 1.0f;

    /** hold time of the running transition */
    float CurrentHoldTime = 1.0f;

    /** timer: end of fade out -> black screen action */
    FTimerHandle BlackScreenTimerHandle;

    /** timer: end of hold -> fade in */
    FTimerHandle FadeInTimerHandle;

    /** timer: end of fade in -> give control back */
    FTimerHandle FinishTimerHandle;

    /** shared flow of all transitions: fade out, action, hold, fade in, unlock */
    void PlayFadeSequence(TFunction<void()> ActionAtBlack, float FadeDuration, float HoldTime);

    /** runs the action while the screen is black and schedules the fade in */
    void OnScreenBlack();

    /** starts the fade in and schedules the unlock */
    void OnStartFadeIn();

    /** unlocks input and, if no dialogue/bark is open, puts the player back in gameplay mode */
    void FinishCinematic();

    /** ignores or restores move/look input of player 0, only once per transition */
    void BlockPlayerInput(bool bBlock);

    /** camera fade to black (1) or to visible (0) */
    void PerformCameraFade(float FromAlpha, float ToAlpha, float Duration);

    /** transform of the first actor with DestinationPointTag, false if none exists */
    bool FindDestination(FName DestinationPointTag, FTransform& OutTransform) const;
};
