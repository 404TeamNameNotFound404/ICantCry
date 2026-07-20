// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_CompleteQuest.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"



void UEvent_CompleteQuest::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (!Player) return;

    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (!QM)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_CompleteQuest: UQuestManagerSystem not found in the GameInstance!"));
        return;
    }

    if (bForceComplete)
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_CompleteQuest: FORCE the completion of %s"), *QuestToComplete.ToString());
        QM->ForceCompleteQuest(QuestToComplete);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_CompleteQuest: normal completion of %s"), *QuestToComplete.ToString());
        QM->TryCompleteQuest(QuestToComplete);
    }
}