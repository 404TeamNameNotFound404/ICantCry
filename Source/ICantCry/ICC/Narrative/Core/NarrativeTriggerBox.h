// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "NarrativeTriggerBox.generated.h"


class UBoxComponent;
class UDialogueAsset;
class AICC_Player;
class UQuestDefinition;

/** Which widget is used to display the trigger's text */
UENUM(BlueprintType)
enum class ENarrativeFeedbackType : uint8
{
    Bark        UMETA(DisplayName = "Bark (quick dialogue)"),
    DialogueUI  UMETA(DisplayName = "Dialogue UI (full dialogue widget)")
};

/**
 * CLASS: ANarrativeTriggerBox
 * DESCRIPTION: a volume that acts as the "gatekeeper" of a narrative door. While the unlock
 * condition (quest completed or key in inventory) is not met, it shows the "it's locked" text
 * when the player enters. Once the condition is met the trigger box disables itself and shows
 * nothing more, so the player is never told the door is locked again. The door actor itself is
 * never touched: this box only manages the message. Designers choose whether the text appears
 * through the Bark widget or the full Dialogue UI via FeedbackType. The widget classes are the
 * ones already configured on the player, so there is nothing else to set up per door.
 */
UCLASS()
class ICANTCRY_API ANarrativeTriggerBox : public AActor
{
    GENERATED_BODY()

public:

    ANarrativeTriggerBox();

protected:

    virtual void BeginPlay() override;

    /** Collision volume that detects the player entering */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> TriggerZone;

    /** Chooses whether the text is shown through the Bark widget or the full Dialogue UI */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
    ENarrativeFeedbackType FeedbackType = ENarrativeFeedbackType::Bark;

    /** Dialogue asset shown while the door is still locked (e.g. "it won't budge") */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
    TObjectPtr<UDialogueAsset> DialogueToTrigger;

    /** If true the "locked" message plays only once; if false it plays on every entry while still locked */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
    bool bTriggerOnce = true;

    /** If set, the door counts as unlocked once this quest is completed */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Unlock")
    TObjectPtr<UQuestDefinition> RequiredQuest;

    /** If valid, the door counts as unlocked while the player owns at least one item with this tag (e.g. a key) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Unlock")
    FGameplayTag RequiredItemTag;

    /** Optional message shown the first time the door unlocks (e.g. "the lock gives way") */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Unlock")
    TObjectPtr<UDialogueAsset> UnlockedBark;

private:

    /** Called when something enters the volume: decides between unlocking and playing the locked message */
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    /** Called when something leaves the volume: clears the "player inside" flag */
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /** Returns true if an unlock condition is set AND satisfied (quest completed or key owned) */
    bool IsUnlocked(AICC_Player* Player) const;

    /** Unlocks the door: shows the optional unlock message and disables this trigger box for good */
    void Unlock(AICC_Player* Player);

    /** Shows the given dialogue through the widget chosen by FeedbackType, stopping the player's movement */
    void PlayFeedback(UDialogueAsset* DialogueToPlay, AICC_Player* Player);

    /** True once the "locked" message has played at least one time, drives bTriggerOnce */
    bool bHasBeenTriggered = false;

    /** True while the player is inside the volume, prevents re-triggering from the same entry */
    bool bPlayerInside = false;

    /** True once the box has been disabled after unlocking, makes the volume inert */
    bool bIsUnlocked = false;

};
