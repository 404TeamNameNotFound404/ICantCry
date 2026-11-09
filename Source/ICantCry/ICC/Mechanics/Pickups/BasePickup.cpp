// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickup.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"


void ABasePickup::Collect(AICC_Player* Player)
{
    if (!Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("BasePickup::Collect - Player NULL su %s, raccolta ignorata"), *GetName());
        return;
    }

    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());
    if (!GI)
    {
        // without the GameInstance neither inventory nor quests can be updated: keep the item in the world
        UE_LOG(LogTemp, Error, TEXT("BasePickup::Collect - UICantCryGameInstance non trovato, %s non raccolto"), *GetName());
        return;
    }

    // inventory and quest progress are independent on purpose:
    // the designer decides per quest whether to use Pick Up, Deliver, or both
    if (bShouldBeStored)
    {
        if (ItemTag.IsValid())
        {
            GI->AddToInventory(ItemTag, AmountToAdd);
        }
        else
        {
            // silent failure here would make any later Deliver objective impossible to complete
            UE_LOG(LogTemp, Warning, TEXT("BasePickup::Collect - %s ha bShouldBeStored=true ma ItemTag e' None: non va in inventario e non potra' essere consegnato"), *GetName());
        }
    }

    if (TargetObjectiveTag.IsValid())
    {
        if (!TargetQuestTag.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("BasePickup::Collect - %s ha TargetObjectiveTag ma TargetQuestTag e' None"), *GetName());
        }
        else if (UQuestManagerSystem* QuestManager = GI->GetSubsystem<UQuestManagerSystem>())
        {
            // if the quest is not active yet the manager ignores the update; the item stays in the inventory anyway
            QuestManager->UpdateObjectiveProgress(TargetQuestTag, TargetObjectiveTag, AmountToAdd);
            DebugHelper::LogSuccess(FString::Printf(TEXT("Item Collected! Objective: %s"), *TargetObjectiveTag.ToString()));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BasePickup::Collect - QuestManagerSystem non trovato"));
        }
    }

    Destroy();
}
