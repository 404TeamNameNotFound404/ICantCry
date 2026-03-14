// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestDefinition.generated.h"

class UGameplayEvent;

/**
 * STRUTTURA: FQuestObjective
 * DESCRIZIONE: Un singolo step di una missione.
 */
USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag ObjectiveID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RequiredCount = 1;
};


/**
 * CLASSE: UQuestDefinition
 * DESCRIZIONE: Definisce una missione, i suoi obiettivi e le ricompense finali.
 */
UCLASS(BlueprintType)
class ICANTCRY_API UQuestDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FGameplayTag QuestID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FText Title;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
    TArray<FQuestObjective> Objectives;

    // Eventi eseguiti al completamento (es. dare oro, XP)
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Rewards")
    TArray<TObjectPtr<UGameplayEvent>> OnCompleteRewards;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FText QuestDescription; 

    // Tag per la categoria (es: Quest.Type.Main o Quest.Type.Side)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FGameplayTag QuestTypeTag;

    UPROPERTY(EditDefaultsOnly, Category = "Quest")
    bool bRequiresNPCTurnIn = true;

    // Opzionale: Il nome dell'NPC da mostrare nel messaggio di ritorno
    UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (EditCondition = "bRequiresNPCTurnIn"))
    FText TargetNPCName;
	
};
