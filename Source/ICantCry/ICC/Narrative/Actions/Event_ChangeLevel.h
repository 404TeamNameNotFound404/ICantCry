// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "GameplayTagContainer.h"
#include "Event_ChangeLevel.generated.h"


class AICC_Player;
class USceneLoader;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ICANTCRY_API UEvent_ChangeLevel : public UGameplayEvent
{
	GENERATED_BODY()
	
public:
	// // Ora selezioni il livello da una lista di Tag (es: Livelli.MappaCitta)
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
    // FGameplayTag LevelTag;

    // // Ora selezioni lo spawn da una lista (es: Spawn.PortaSud)
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Transition")
    // FGameplayTag PlayerStartTag;

	// virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;



    // Se spuntato, il player apparirà sul PlayerStart con il tag scelto.
    // Se NON spuntato, verrà usata la posizione salvata (come dopo una battaglia).
    UPROPERTY(EditAnywhere, Category = "Level Transition")
    bool bUsePlayerStart = true;

    // Seleziona la mappa di destinazione
    UPROPERTY(EditAnywhere, Category = "Level Transition")
    FGameplayTag LevelTag;

    // Seleziona lo spawn point (Visibile solo se bUsePlayerStart è attivo)
    UPROPERTY(EditAnywhere, Category = "Level Transition", meta = (EditCondition = "bUsePlayerStart"))
    FGameplayTag PlayerStartTag;

    virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
