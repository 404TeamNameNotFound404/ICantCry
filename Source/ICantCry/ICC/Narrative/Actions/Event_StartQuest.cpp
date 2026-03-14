// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_StartQuest.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UEvent_StartQuest::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	if (!Player) return;

    // IMPORTANTE: Usa GetGameInstance() perché ora è un GameInstanceSubsystem
    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    
    if (QM && QuestToStart)
    {
        // Chiamiamo AcceptQuest definita nel tuo QuestManagerSystem.h
        QM->AcceptQuest(QuestToStart);
        UE_LOG(LogTemp, Warning, TEXT("Quest %s avviata correttamente!"), *QuestToStart->Title.ToString());
    }
}