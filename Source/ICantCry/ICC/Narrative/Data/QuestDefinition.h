// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestDefinition.generated.h"

class UGameplayEvent;

/**
 * STRUCTURE: FQuestObjective
 * DESCRIPTION: a single step in a quest, tracked by the quest manager
 * each objective has a unique id, a description for ui, and a required count
 */
USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    /** unique identifier for this objective, used by the quest manager to track progress */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag ObjectiveID;

    /** text shown in the quest log describing what the player needs to do */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    /** how many times the player needs to do this (kill 5 rats, collect 3 items, etc) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RequiredCount = 1;
};


/**
 * CLASS: UQuestDefinition
 * DESCRIPTION: defines a quest, its objectives, and final rewards
 * this is a data asset that designers create for each quest in the game
 */
UCLASS(BlueprintType)
class ICANTCRY_API UQuestDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** unique gameplay tag that identifies this quest, used by the quest manager for all lookups */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FGameplayTag QuestID;

    /** short name of the quest shown in ui and logs */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FText Title;

    /** list of objectives that must be completed to finish the quest */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
    TArray<FQuestObjective> Objectives;

    /** events executed when the quest is completed (give gold, xp, items, etc) */
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Rewards")
    TArray<TObjectPtr<UGameplayEvent>> OnCompleteRewards;

    /** full description shown in the quest log, can be longer than the title */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FText QuestDescription; 

    /** category tag for filtering quests in ui (main quest, side quest, faction quest, etc) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
    FGameplayTag QuestTypeTag;

    /** if true, player must talk to an npc to turn in the quest
        if false, quest completes automatically when objectives are done */
    UPROPERTY(EditDefaultsOnly, Category = "Quest")
    bool bRequiresNPCTurnIn = true;

    /** optional: name of the npc to return to, shown in ui when quest is ready to turn in */
    UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (EditCondition = "bRequiresNPCTurnIn"))
    FText TargetNPCName;
	
};
