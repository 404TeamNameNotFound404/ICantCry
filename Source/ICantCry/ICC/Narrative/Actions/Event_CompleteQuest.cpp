// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_CompleteQuest.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"



void UEvent_CompleteQuest::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (!Player) return;

    // Poiché ora è un GameInstanceSubsystem, lo cerchiamo tramite il GameInstance
    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    
    if (QM)
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest Manager trovato! Completamento quest: %s"), *QuestToComplete.ToString());
        QM->TryCompleteQuest(QuestToComplete);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ERRORE: Impossibile trovare UQuestManagerSystem nel GameInstance!"));
    }
}