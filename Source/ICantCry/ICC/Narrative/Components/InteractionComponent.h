// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class UDialogueAsset;
class UQuestDefinition;
class UDialogueWidget;
class AICC_Player;



USTRUCT(BlueprintType)
struct FQuestDialogueChain
{
    GENERATED_BODY()

    /** the quest associated with this chain link, we use its tag to check status in the quest manager */
    UPROPERTY(EditAnywhere, Category = "Quest")
    TObjectPtr<UQuestDefinition> Quest;

    /** if TRUE, the dialogue widget will show accept/decline buttons for this quest, otherwise it starts automatically */
    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bIsOptional = false;

    /** dialogue to play when player meets the npc and this quest hasn't been started yet */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> StartDialogue;

    /** dialogue to play when the quest is active but objectives are not all complete yet */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> InProgressDialogue;

    /** dialogue to play when the quest is active and ALL objectives are complete (ready to turn in) */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> CompletedDialogue;
};



/**
 * CLASS: UInteractionComponent
 * DESCRIPTION: component to attach to any actor that should be interactable (npcs, quest givers, etc)
 * handles the logic of selecting the correct dialogue based on quest states in the quest manager
 */
UCLASS(ClassGroup=(Narrative), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	// --- DIALOGUE CONFIGURATION ---
	
	/** 
     * ordered list of quests this npc handles. order matters: 
     * the system iterates through the list until it finds the first quest NOT permanently completed
     * and uses that to determine the dialogue. if all are completed, uses FinalDefaultDialogue
     */
    UPROPERTY(EditAnywhere, Category = "Narrative|Config")
    TArray<FQuestDialogueChain> QuestChain;

    /** fallback dialogue when all quests in the chain have been completed and turned in */
    UPROPERTY(EditAnywhere, Category = "Narrative|Dialogue")
    TObjectPtr<UDialogueAsset> FinalDefaultDialogue;


	/** widget class to instantiate when a dialogue starts, must derive from UDialogueWidget */
	UPROPERTY(EditAnywhere, Category = "Narrative|Config")
	TSubclassOf<class UDialogueWidget> DialogueWidgetClass;

	/** entry point called by the player when they press the interact button on this npc */
	void TriggerInteraction(AICC_Player* Player);
};
