// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BasePickup.generated.h"

UCLASS()
class ICANTCRY_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Funzione chiamata quando il player "tocca" o "interagisce" con l'oggetto
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Collect(AICC_Player* Player);

protected:

	// --- TAG PER IL SISTEMA MISSIONI ---
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetQuestTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (AllowPrivateAccess = "true"))
    FGameplayTag TargetObjectiveTag;


    // Quanti punti dare (es. 1 sasso)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 AmountToAdd = 1;

};
