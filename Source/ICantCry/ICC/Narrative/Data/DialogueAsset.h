// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/Data/PlayerProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "DialogueAsset.generated.h"

/**
 * CLASS: UDialogueAsset
 * DESCRIPTION: data asset that contains a full dialogue sequence, including lines, branching, and visual settings
 * these are referenced by interaction components and played through the dialogue widget
 */

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	/** the actual text spoken */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Text;

    // aggiunta: Determina se questa riga è pronunciata dal Player invece che dall'NPC
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speaker")
    bool bIsPlayerLine = false;

	/** events triggered when this specific line is displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
    TArray<TObjectPtr<class UGameplayEvent>> Events;

	/** emotional state for this line */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EmotionTag;
};

USTRUCT(BlueprintType)
struct FDialogueBranch
{
    GENERATED_BODY()

    /** the text the player sees on the choice button */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText ReplyText;

    /** the dialogue asset that plays if the player picks this branch */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UDialogueAsset* NextDialogue;
};




UCLASS(BlueprintType)
class ICANTCRY_API UDialogueAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:

	// aggiunta: Riferimento opzionale al profilo del player se ci sono linee pronunciate da lui
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profiles")
    TObjectPtr<class UPlayerProfile> PlayerProfile;


	/** which npc is speaking this dialogue, used for name and portraits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<class UNPCProfile> NPCProfile;

	/** the actual lines of dialogue in order */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TArray<FDialogueLine> Lines;

	/** font to use for this dialogue, can override global settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FSlateFontInfo DialogueFont;

	/** text color for this dialogue, white by default */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor DialogueColor = FLinearColor::White;

	/** whether to show text character by character or all at once */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	bool bUseTypewriterEffect = true;

	/** delay between characters when typewriter effect is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bUseTypewriterEffect"))
	float TypewriterSpeed = 0.05f;

	/** events triggered when this dialogue finishes (start quest, give item, etc) */
	UPROPERTY(EditAnywhere, Instanced, Category = "Events Dialogue Ended")
	TArray<TObjectPtr<UGameplayEvent>> OnDialogueEnded;

	/** if true, this dialogue will never show quest buttons even if the npc has quests available
	    useful for purely narrative dialogues where you don't want UI interference */
	UPROPERTY(EditAnywhere, Category = "Config")
    bool bNeverShowQuestButtons = false;

	/** whether this dialogue has branching choices at the end */
	UPROPERTY(EditAnywhere, Category = "Branches")
    bool bUseBranches = false;

    /** list of possible branches the player can choose, only shown if bUseBranches is true */
    UPROPERTY(EditAnywhere, Category = "Branches", meta = (EditCondition = "bUseBranches"))
    TArray<FDialogueBranch> Branches;
	
};
