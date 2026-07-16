// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "QuestManagerSystem.generated.h"

class AICC_Player;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestSystemUpdate);

/**
 * CLASS: UQuestManagerSystem
 * DESCRIPTION: Global subsystem that manages quest activation, objective tracking, and completion.
 * It acts as the "brain" for all narrative progression and communicates with the UI via delegates.
 */

USTRUCT(BlueprintType)
struct FQuestProgress
{
    GENERATED_BODY()

    /** The data definition of the quest (Title, Description, Objectives). */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestDefinition> QuestDef;

    /** Current progress for each objective, mapped by its GameplayTag. */
    UPROPERTY(BlueprintReadOnly)
    TMap<FGameplayTag, int32> ObjectiveProgress;

    FQuestProgress() : QuestDef(nullptr) {}
    FQuestProgress(UQuestDefinition* InDef) : QuestDef(InDef) {}
};


UCLASS()
class ICANTCRY_API UQuestManagerSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// --- API PER IL GIOCO ---
    
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void AcceptQuest(UQuestDefinition* Quest);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void UpdateObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool IsQuestActive(FGameplayTag QuestTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool IsQuestCompleted(FGameplayTag QuestTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AreObjectivesComplete(const FQuestProgress& Progress) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AreObjectivesCompleteByTag(FGameplayTag QuestTag) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool TryCompleteQuest(FGameplayTag QuestTag);

    // FUNZIONE DI SUPPORTO: Trova l'indice di una missione attiva tramite il suo Tag
    UFUNCTION(BlueprintCallable, Category = "Quest")
    int32 FindActiveQuestIndex(FGameplayTag QuestTag) const;

    /** Completes the quest, bypassing the objective check (still grants rewards and removes it).
 *  Works only if the quest is ACTIVE. */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool ForceCompleteQuest(FGameplayTag QuestTag);

    // --- EVENTI ---
    
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestSystemUpdate OnSystemUpdate;

    // Getter per la UI
    const TArray<FQuestProgress>& GetActiveQuests() const { return ActiveQuests; }

    //Per permettere alla UI di sapere quanti oggetti sono già stati consegnati.
   UFUNCTION(BlueprintPure, Category = "Quest")
    int32 GetObjectiveProgress(FGameplayTag QuestTag, FGameplayTag ObjectiveTag) const;

protected:
    UPROPERTY()
    TArray<FQuestProgress> ActiveQuests;

    UPROPERTY()
    FGameplayTagContainer CompletedQuestsTags;


    // Controlla se tutti gli obiettivi sono stati soddisfatti
    void CheckQuestCompletion(int32 QuestIndex);

private:
    void CompleteQuest(int32 Index);
};
