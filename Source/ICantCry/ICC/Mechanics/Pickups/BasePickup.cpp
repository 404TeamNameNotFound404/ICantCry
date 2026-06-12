// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void ABasePickup::Collect(AICC_Player* Player)
{
	if (!Player) return;

    // access the GameInstance to reach both inventory and quest systems
    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());
    if (GI)
    {
        // Handle inventory storage if the item is persistent
        if (bShouldBeStored && ItemTag.IsValid())
        {
            GI->AddToInventory(ItemTag, AmountToAdd);
        }

       
        UQuestManagerSystem* QuestManager = GI->GetSubsystem<UQuestManagerSystem>();
        if (QuestManager)
        {
            // upd the specific quest objective using the assigned tags
            QuestManager->UpdateObjectiveProgress(TargetQuestTag, TargetObjectiveTag, AmountToAdd);
            
            // Log 
            DebugHelper::LogSuccess(FString::Printf(TEXT("Item Collected! Objective: %s"), *TargetObjectiveTag.ToString()));
        }
    }

    
    Destroy();
}
