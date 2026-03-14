// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "NPCProfile.generated.h"

/**
 * CLASSE: UNPCProfile
 * DESCRIZIONE: Contiene i dati identificativi di un NPC.
 */
UCLASS(BlueprintType)
class ICANTCRY_API UNPCProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Nome del personaggio che apparirà nei dialoghi
	// Nome del personaggio visibile nella UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
    FText CharacterName;

	// Mappa delle emozioni (Tag) e delle relative immagini (Texture)
	// Esempio: "Emozione.Felice" -> Texture_Paolo_Happy
	// Associazione tra Tag emozione e Texture (es: "Emozione.Triste" -> T_Paolo_Sad)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> Portraits;
};

	

