// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSystem.h"
// #include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UQuestManagerSystem::AcceptQuest(UQuestDefinition* Quest)
{
    if (!Quest || IsQuestActive(Quest->QuestID) || IsQuestCompleted(Quest->QuestID)) return;

    ActiveQuests.Add(FQuestProgress(Quest));
    
    // Notifica la UI di aggiornarsi
    OnSystemUpdate.Broadcast();
}

void UQuestManagerSystem::UpdateObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag, int32 Amount)
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
        ActiveQuests[Index].ObjectiveProgress.FindOrAdd(ObjectiveTag) += Amount;

        // Se gli obiettivi sono finiti e NON serve l'NPC, chiudiamo subito
        if (AreObjectivesComplete(ActiveQuests[Index]) && !ActiveQuests[Index].QuestDef->bRequiresNPCTurnIn)
        {
            CompleteQuest(Index);
        }
        else
        {
            // Altrimenti resta lì, ma la UI deve sapere che è cambiato qualcosa
            OnSystemUpdate.Broadcast();
        }
    }
}


bool UQuestManagerSystem::AreObjectivesComplete(const FQuestProgress& Progress) const
{
    if (!Progress.QuestDef) return false;
    
    for (const FQuestObjective& Obj : Progress.QuestDef->Objectives)
    {
        // FindRef è perfetto perché se il tag non esiste nella mappa restituisce 0
        int32 CurrentValue = Progress.ObjectiveProgress.FindRef(Obj.ObjectiveID);
        if (CurrentValue < Obj.RequiredCount) return false;
    }
    return true;
}

bool UQuestManagerSystem::AreObjectivesCompleteByTag(FGameplayTag QuestTag) const
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
        // Chiama la funzione sopra passando i dati della missione trovata
        return AreObjectivesComplete(ActiveQuests[Index]);
    }
    return false;
}

bool UQuestManagerSystem::IsQuestActive(FGameplayTag QuestTag) const
{
   return FindActiveQuestIndex(QuestTag) != INDEX_NONE;
}

bool UQuestManagerSystem::IsQuestCompleted(FGameplayTag QuestTag) const
{
    return CompletedQuestsTags.HasTagExact(QuestTag);
}

void UQuestManagerSystem::CompleteQuest(int32 Index)
{
//    FQuestProgress CompletedQuest = ActiveQuests[Index];
    
//     // 1. Recuperiamo il Player e facciamo il cast alla tua classe specifica
//     AICC_Player* PlayerCharacter = Cast<AICC_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());
//     // 1. Aggiungi ai completati
//     if (CompletedQuest.QuestDef)
//     {
//         CompletedQuestsTags.AddTag(CompletedQuest.QuestDef->QuestID);
//     }

//    // 2. Eseguiamo le ricompense passando i DUE argomenti richiesti
//    for (UGameplayEvent* Reward : CompletedQuest.QuestDef->OnCompleteRewards)
//     {
//         if (Reward && PlayerCharacter) 
//         {
//             // Ora passiamo i DUE argomenti: il Player e la Quest come contesto
//             Reward->ExecuteEvent(PlayerCharacter, CompletedQuest.QuestDef);
//         }
//     }

//     // 3. Rimuovi dalle attive
//     ActiveQuests.RemoveAt(Index);

//     // 4. Update UI
//     OnSystemUpdate.Broadcast();
    
//     UE_LOG(LogTemp, Warning, TEXT("Missione Completata: %s"), *CompletedQuest.QuestDef->Title.ToString());


    // 0. Controllo sicurezza Indice: Fondamentale per evitare crash
    if (!ActiveQuests.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("SYSTEM ERROR: Tentativo di completare missione con Indice non valido: %d"), Index);
        return;
    }

    FQuestProgress CompletedQuest = ActiveQuests[Index];
    
    // Controllo che il Definition sia valido
    if (!CompletedQuest.QuestDef) 
    {
        UE_LOG(LogTemp, Error, TEXT("SYSTEM ERROR: La missione all'indice %d non ha un QuestDef valido!"), Index);
        return;
    }

    // 1. Recuperiamo il Player
    AICC_Player* PlayerCharacter = Cast<AICC_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

    // 2. Aggiungi ai completati (Tag)
    // Usiamo il QuestID univoco del Data Asset
    CompletedQuestsTags.AddTag(CompletedQuest.QuestDef->QuestID);

    // 3. Eseguiamo le ricompense
    for (UGameplayEvent* Reward : CompletedQuest.QuestDef->OnCompleteRewards)
    {
        if (Reward && PlayerCharacter) 
        {
            // Passiamo il Player (bersaglio) e la Quest (contesto)
            Reward->ExecuteEvent(PlayerCharacter, CompletedQuest.QuestDef);
        }
    }

    // 4. Rimuovi dalle attive
    // Facciamo questo PRIMA del broadcast così la UI leggerà l'array già aggiornato
    ActiveQuests.RemoveAt(Index);

    // 5. Update UI
    // Questo è il comando che "spegne" la riga nella tua CharacterUI
    OnSystemUpdate.Broadcast();
    
    UE_LOG(LogTemp, Warning, TEXT("=== SYSTEM: Missione '%s' (ID: %s) Rimossa dalle attive e completata! ==="), 
        *CompletedQuest.QuestDef->Title.ToString(),
        *CompletedQuest.QuestDef->QuestID.ToString());
}


void UQuestManagerSystem::CheckQuestCompletion(int32 QuestIndex)
{
    FQuestProgress& Progress = ActiveQuests[QuestIndex];
    bool bAllFinished = true;

    // Cicla tutti gli obiettivi definiti nel Data Asset
    for (const FQuestObjective& Obj : Progress.QuestDef->Objectives)
    {
        int32 CurrentValue = Progress.ObjectiveProgress.FindOrAdd(Obj.ObjectiveID);
        if (CurrentValue < Obj.RequiredCount)
        {
            bAllFinished = false;
            break;
        }
    }

    if (bAllFinished)
    {
        CompleteQuest(QuestIndex);
    }
}


int32 UQuestManagerSystem::FindActiveQuestIndex(FGameplayTag QuestTag) const
{
    for (int32 i = 0; i < ActiveQuests.Num(); ++i)
    {
        if (ActiveQuests[i].QuestDef && ActiveQuests[i].QuestDef->QuestID.MatchesTag(QuestTag))
        {
            return i;
        }
    }
    return INDEX_NONE;
}


bool UQuestManagerSystem::TryCompleteQuest(FGameplayTag QuestTag)
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    
    if (Index != INDEX_NONE)
    {
        // Verifichiamo se gli obiettivi sono effettivamente tutti completi
        if (AreObjectivesComplete(ActiveQuests[Index]))
        {
            // Tutto okay! Chiamiamo la funzione di chiusura che rimuove la quest e dà i premi
            CompleteQuest(Index);
            return true;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Tentativo di completare %s fallito: Obiettivi non pronti o quest non attiva."), *QuestTag.ToString());
    return false;
}

int32 UQuestManagerSystem::GetObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag) const
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
        const int32* Progress = ActiveQuests[Index].ObjectiveProgress.Find(ObjectiveTag);
        return Progress ? *Progress : 0;
    }
    return 0;
}