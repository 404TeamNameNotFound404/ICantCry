// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "DialogueAsset.generated.h"



USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EmotionTag; // Per cambiare l'immagine dell'NPC nel Widget
};

USTRUCT(BlueprintType)
struct FDialogueBranch
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText ReplyText; // Quello che il giocatore legge sul bottone

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UDialogueAsset* NextDialogue; // Il dialogo che parte se scegli questa risposta
};


UCLASS(BlueprintType)
class ICANTCRY_API UDialogueAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	UNPCProfile* NPCProfile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	TArray<FDialogueLine> Lines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FSlateFontInfo DialogueFont;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FLinearColor DialogueColor = FLinearColor::White; // Default bianco

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	bool bUseTypewriterEffect = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance", meta = (EditCondition = "bUseTypewriterEffect"))
	float TypewriterSpeed = 0.05f;

	// Eventi da scatenare alla fine del dialogo (es: Inizia Quest o Drop Item)
	UPROPERTY(EditAnywhere, Instanced, Category = "Events")
	TArray<TObjectPtr<UGameplayEvent>> OnDialogueEnded;

	UPROPERTY(EditAnywhere, Category = "Config")
    bool bNeverShowQuestButtons = false;

	UPROPERTY(EditAnywhere, Category = "Branches")
    bool bUseBranches = false;

    UPROPERTY(EditAnywhere, Category = "Branches", meta = (EditCondition = "bUseBranches"))
    TArray<FDialogueBranch> Branches;


	
};
