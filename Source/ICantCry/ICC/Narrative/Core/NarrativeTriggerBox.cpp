// Fill out your copyright notice in the Description page of Project Settings.


#include "NarrativeTriggerBox.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"


ANarrativeTriggerBox::ANarrativeTriggerBox()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
    RootComponent = TriggerZone;

    TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
}

void ANarrativeTriggerBox::BeginPlay()
{
    Super::BeginPlay();

    TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ANarrativeTriggerBox::OnOverlapBegin);
    TriggerZone->OnComponentEndOverlap.AddDynamic(this, &ANarrativeTriggerBox::OnOverlapEnd);
}

void ANarrativeTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    AICC_Player* Player = Cast<AICC_Player>(OtherActor);
    if (!Player) return;

    // One trigger per entry: if the player is already inside (a second component overlapping,
    // or the player pushing further in) we do nothing.
    if (bPlayerInside) return;
    bPlayerInside = true;

    // Already unlocked during this session: the volume is inert, the player just walks through.
    if (bIsUnlocked) return;

    // Condition satisfied: open up and disable, instead of repeating the "locked" message.
    if (IsUnlocked(Player))
    {
        Unlock(Player);
        return;
    }

    // Still locked: show the "it's blocked" message. bTriggerOnce keeps it to a single reminder.
    if (bTriggerOnce && bHasBeenTriggered) return;
    bHasBeenTriggered = true;

    PlayFeedback(DialogueToTrigger, Player);
}

void ANarrativeTriggerBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!Cast<AICC_Player>(OtherActor)) return;

    // Clear the flag only when no component of the player overlaps the box anymore,
    // otherwise a player with several collision shapes would report a false "left the volume".
    if (!TriggerZone->IsOverlappingActor(OtherActor))
    {
        bPlayerInside = false;
    }
}

bool ANarrativeTriggerBox::IsUnlocked(AICC_Player* Player) const
{
    if (!Player) return false;

    // Quest condition: unlocked once the required quest is completed.
    if (RequiredQuest)
    {
        if (UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>())
        {
            if (QM->IsQuestCompleted(RequiredQuest->QuestID))
            {
                return true;
            }
        }
    }

    // Key condition: unlocked while the player owns at least one item with the required tag.
    if (RequiredItemTag.IsValid())
    {
        if (UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance()))
        {
            if (GI->GetItemCount(RequiredItemTag) > 0)
            {
                return true;
            }
        }
    }

    // No condition set at all: this box never unlocks and stays a plain narrative trigger.
    return false;
}

void ANarrativeTriggerBox::Unlock(AICC_Player* Player)
{
    bIsUnlocked = true;

    // Optional opening message, shown once, through the same widget the designer picked.
    if (UnlockedBark)
    {
        PlayFeedback(UnlockedBark, Player);
    }

    // Disable the trigger box: no more overlap callbacks, no more messages.
    // The door actor is deliberately left untouched.
    TriggerZone->SetGenerateOverlapEvents(false);
}

void ANarrativeTriggerBox::PlayFeedback(UDialogueAsset* DialogueToPlay, AICC_Player* Player)
{
    if (!DialogueToPlay || !Player) return;

    // Stop the player so they don't keep sliding while the text pops up.
    if (Player->GetCharacterMovement())
    {
        Player->GetCharacterMovement()->StopMovementImmediately();
    }

    // We delegate to the player instead of creating the widget here: those functions already
    // queue the text when a menu (Bestiary / InGameMenu) is open, and they reuse the widget
    // classes configured once on the player, so there is nothing to set per door.
    if (FeedbackType == ENarrativeFeedbackType::DialogueUI)
    {
        Player->PlayDialogueUIImmediately(DialogueToPlay);
    }
    else
    {
        Player->PlayBarkImmediately(DialogueToPlay);
    }
}
