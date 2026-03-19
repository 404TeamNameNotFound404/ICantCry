// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

UInteractionComponent::UInteractionComponent() 
{ 
	PrimaryComponentTick.bCanEverTick = false; 
}


void UInteractionComponent::TriggerInteraction(AICC_Player* Player)
{

   // grab the quest manager from game instance, it's a subsystem so this is safe
    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    
	// start with null instead of default dialogue
    // if dialogue to play is still null after the loop, the npc does nothing
    UDialogueAsset* DialogueToPlay = nullptr; 
    bool bOptional = false;

    if (QM)
    {
        bool bAllQuestsFinished = true;

        // iterate through the chain in order, stopping at the first quest that isn't permanently completed
        for (const FQuestDialogueChain& Step : QuestChain)
        {
            if (!Step.Quest) continue;
            FGameplayTag QID = Step.Quest->QuestID;

            // if the quest is already completed and turned in, skip to the next chain link
			// this lets us have multiple quests from the same npc in sequence
            if (QM->IsQuestCompleted(QID))
            {
                continue; 
            }

            // we found the current quest (the one the player is stuck on or can start)
            bAllQuestsFinished = false;

            if (QM->IsQuestActive(QID))
            {
                // quest is active, check if objectives are done to determine if it's turn-in time
                if (QM->AreObjectivesCompleteByTag(QID)) 
                {
                    DialogueToPlay = Step.CompletedDialogue;
                }
                else 
                {
                    DialogueToPlay = Step.InProgressDialogue;
                }
            }
            else
            {
                // quest hasn't been started yet, use the start dialogue
                DialogueToPlay = Step.StartDialogue;
                bOptional = Step.bIsOptional;
            }

            // found our current quest, stop iterating so we don't accidentally pick a later one
            break; 
        }

        // if we went through the whole chain and all quests are completed...
        if (bAllQuestsFinished)
        {
            // ...use the final "dumb" dialogue the designers set up
            DialogueToPlay = FinalDefaultDialogue;
        }
    }

    // only show the widget if we actually have dialogue to play
    // if null (because final default dialogue is empty), the player presses the button but nothing happens
	// this is intentional so designers can have npcs that become silent after all quests are done
    if (DialogueToPlay)
    {
        UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
        if (DialogueWidget)
        {
            DialogueWidget->AddToViewport();
            DialogueWidget->SetIsOptional(bOptional);
            DialogueWidget->StartDialogue(DialogueToPlay);
        }
    }
    else 
    {
        // optional: log or play a small sound feedback if you want
		// this helps with debugging so you know the npc was interacted with but had nothing to say
        UE_LOG(LogTemp, Log, TEXT("npc has nothing left to say"));
    }
}
