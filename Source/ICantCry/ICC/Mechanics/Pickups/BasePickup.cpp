// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickup.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void ABasePickup::Collect(AICC_Player* Player)
{
	if (!Player) return;

    // 1. Recuperiamo il Manager tramite il GameInstance Subsystem
   UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());
    if (GI)
    {

        // --- NUOVA LOGICA DI STIVAGGIO ---
        if (bShouldBeStored && ItemTag.IsValid())
        {
            GI->AddToInventory(ItemTag, AmountToAdd);
        }


        UQuestManagerSystem* QuestManager = GI->GetSubsystem<UQuestManagerSystem>();
        if (QuestManager)
        {
            // 2. Chiamiamo la funzione del Manager con i TAG corretti
            QuestManager->UpdateObjectiveProgress(TargetQuestTag, TargetObjectiveTag, AmountToAdd);
            
            DebugHelper::LogSuccess(FString::Printf(TEXT("Sasso Raccolto! Obiettivo: %s"), *TargetObjectiveTag.ToString()));
        }
    }

    // 3. Il sasso scompare dal mondo
    Destroy();
}
