// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSystem.h"
// #include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UQuestManagerSystem::AcceptQuest(UQuestDefinition* Quest)
{
	// ignore if quest is invalid, already active, or already completed
	// prevents double-accepting or restarting finished quests
    if (!Quest || IsQuestActive(Quest->QuestID) || IsQuestCompleted(Quest->QuestID)) return;

    ActiveQuests.Add(FQuestProgress(Quest));
    
    // tell the ui to refresh so the new quest appears in the journal
    OnSystemUpdate.Broadcast();
}

void UQuestManagerSystem::UpdateObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag, int32 Amount)
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
		// find or add initializes to 0 if the objective hasn't been tracked yet
		// this is important because some objectives might start at 0 implicitly
        ActiveQuests[Index].ObjectiveProgress.FindOrAdd(ObjectiveTag) += Amount;

        // if objectives are done and the quest doesn't require npc turn-in, complete it immediately
		// this covers kill quests, collection quests, etc where you don't need to talk to anyone
        if (AreObjectivesComplete(ActiveQuests[Index]) && !ActiveQuests[Index].QuestDef->bRequiresNPCTurnIn)
        {
            CompleteQuest(Index);
        }
        else
        {
            // otherwise just update the ui so it shows new progress numbers
            OnSystemUpdate.Broadcast();
        }
    }
}


bool UQuestManagerSystem::AreObjectivesComplete(const FQuestProgress& Progress) const
{
    if (!Progress.QuestDef) return false;
    
    for (const FQuestObjective& Obj : Progress.QuestDef->Objectives)
    {
        // findref returns 0 if the tag doesn't exist in the map, which is exactly what we want
		// objectives with no progress are treated as 0, so they fail the >= check automatically
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
        // call the function above with the found quest data
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
	// has tagexact ensures we match exactly, not just parent tags
	// quest ids should be unique so this is safe
    return CompletedQuestsTags.HasTagExact(QuestTag);
}

void UQuestManagerSystem::CompleteQuest(int32 Index)
{
 //   // safety check: index must be valid to avoid crashes
	//// this can happen if something calls completequest with an index that was already removed
 //   if (!ActiveQuests.IsValidIndex(Index))
 //   {
 //       UE_LOG(LogTemp, Error, TEXT("system error: tried to complete quest with invalid index: %d"), Index);
 //       return;
 //   }

 //   FQuestProgress CompletedQuest = ActiveQuests[Index];
 //   
 //   // make sure the definition is valid
	//// if this happens, something corrupted the active quests array
 //   if (!CompletedQuest.QuestDef) 
 //   {
 //       UE_LOG(LogTemp, Error, TEXT("system error: quest at index %d has no valid questdef!"), Index);
 //       return;
 //   }

 //   // grab the player for reward delivery
	//// we need the player to execute gameplay events that affect the character
 //   AICC_Player* PlayerCharacter = Cast<AICC_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

 //   // add to completed tags using the unique quest id from the data asset
	//// this prevents the quest from being accepted again
 //   CompletedQuestsTags.AddTag(CompletedQuest.QuestDef->QuestID);

 //   // execute all rewards
	//// rewards can be anything: give items, unlock abilities, change world state, etc
 //   for (UGameplayEvent* Reward : CompletedQuest.QuestDef->OnCompleteRewards)
 //   {
 //       if (Reward && PlayerCharacter) 
 //       {
 //           // pass the player (target) and the quest (context)
	//		// the quest context is useful for rewards that need to know which quest just completed
 //           Reward->ExecuteEvent(PlayerCharacter, CompletedQuest.QuestDef);
 //       }
 //   }

 //   // remove from active quests
 //   // do this BEFORE the broadcast so the ui reads the updated array
	//// if we broadcast first, the ui would show the quest for one more frame
 //   ActiveQuests.RemoveAt(Index);

 //   // tell ui to refresh (this removes the quest line from character ui)
 //   OnSystemUpdate.Broadcast();
 //   
 //   UE_LOG(LogTemp, Warning, TEXT("=== system: quest '%s' (id: %s) removed from active and completed! ==="), 
 //       *CompletedQuest.QuestDef->Title.ToString(),
 //       *CompletedQuest.QuestDef->QuestID.ToString());



    // Controllo di sicurezza: l'indice deve essere valido per evitare crash.
    if (!ActiveQuests.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("system error: tried to complete quest with invalid index: %d"), Index);
        return;
    }

    FQuestProgress CompletedQuest = ActiveQuests[Index];

    // La definizione deve essere valida, altrimenti l'array � corrotto.
    if (!CompletedQuest.QuestDef)
    {
        UE_LOG(LogTemp, Error, TEXT("system error: quest at index %d has no valid questdef!"), Index);
        return;
    }

    // --- INIZIO PARTE MODIFICATA ---

    // Risoluzione robusta del player per i reward: proviamo il primo player controller,
    // poi il fallback via GameplayStatics. Cos� i reward non vengono saltati se la
    // risoluzione diretta torna null.
    AICC_Player* PlayerCharacter = nullptr;
    if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        PlayerCharacter = Cast<AICC_Player>(PC->GetPawn());
    }
    if (!PlayerCharacter)
    {
        PlayerCharacter = Cast<AICC_Player>(UGameplayStatics::GetPlayerPawn(this, 0));
    }

    // Marca la quest come completata (usando il QuestID unico) per impedirne il ri-accept.
    CompletedQuestsTags.AddTag(CompletedQuest.QuestDef->QuestID);

    // Log diagnostico: quante reward ci sono e se il player � stato risolto.
    const int32 RewardCount = CompletedQuest.QuestDef->OnCompleteRewards.Num();
    UE_LOG(LogTemp, Log, TEXT("CompleteQuest: eseguo %d reward per '%s' (Player=%s)"),
        RewardCount, *CompletedQuest.QuestDef->QuestID.ToString(),
        PlayerCharacter ? *PlayerCharacter->GetName() : TEXT("NULL"));

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("CompleteQuest: player non risolto, i reward verranno saltati."));
    }

    // Esegue tutte le reward (dare oggetti, exp, cambiare stato del mondo, ecc.).
    for (UGameplayEvent* Reward : CompletedQuest.QuestDef->OnCompleteRewards)
    {
        if (!Reward)
        {
            UE_LOG(LogTemp, Warning, TEXT("CompleteQuest: entry NULLA in OnCompleteRewards."));
            continue;
        }
        if (!PlayerCharacter) break;

        // Passiamo il player (target) e la quest (context).
        Reward->ExecuteEvent(PlayerCharacter, CompletedQuest.QuestDef);
    }

    // --- FINE PARTE MODIFICATA ---

    // Rimuove dalla lista attive PRIMA del broadcast, cos� la UI legge l'array aggiornato.
    ActiveQuests.RemoveAt(Index);

    // Aggiorna la UI (rimuove la riga della quest).
    OnSystemUpdate.Broadcast();

    UE_LOG(LogTemp, Warning, TEXT("=== system: quest '%s' (id: %s) removed from active and completed! ==="),
        *CompletedQuest.QuestDef->Title.ToString(),
        *CompletedQuest.QuestDef->QuestID.ToString());
}


void UQuestManagerSystem::CheckQuestCompletion(int32 QuestIndex)
{
	// this is called when we suspect a quest might be complete
	// usually after updating an objective
    FQuestProgress& Progress = ActiveQuests[QuestIndex];
    bool bAllFinished = true;

    // loop through all objectives defined in the data asset
    for (const FQuestObjective& Obj : Progress.QuestDef->Objectives)
    {
		// find or add to be safe, though progress should already exist for objectives we track
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
	// linear search because active quests array is small (usually < 20)
	// if we had hundreds of active quests we'd need a map, but that's overkill here
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
	// this is called by npc interaction when player tries to turn in a quest
    int32 Index = FindActiveQuestIndex(QuestTag);
    
    if (Index != INDEX_NONE)
    {
        // verify that objectives are actually all complete
		// prevents cheating or npc giving reward when objectives aren't done
        if (AreObjectivesComplete(ActiveQuests[Index]))
        {
            // all good! call the completion function that removes the quest and gives rewards
            CompleteQuest(Index);
            return true;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("attempt to complete %s failed: objectives not ready or quest not active."), *QuestTag.ToString());
    return false;
}

int32 UQuestManagerSystem::GetObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag) const
{
	// used by ui to display progress bars or counters
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
        const int32* Progress = ActiveQuests[Index].ObjectiveProgress.Find(ObjectiveTag);
        return Progress ? *Progress : 0;
    }
    return 0;
}


bool UQuestManagerSystem::ForceCompleteQuest(FGameplayTag QuestTag)
{
    int32 Index = FindActiveQuestIndex(QuestTag);
    if (Index != INDEX_NONE)
    {
        // CompleteQuest non controlla gli obiettivi: d� i reward e rimuove la quest.
        CompleteQuest(Index);
        return true;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("ForceCompleteQuest: '%s' non è attiva, impossibile forzarla (nessuna QuestDef/reward disponibili)."),
        *QuestTag.ToString());
    return false;
}