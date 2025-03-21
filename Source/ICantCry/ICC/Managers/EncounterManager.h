// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Actors/Player/ICC_Player.h"
#include "EncounterManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerLocation : uint8
{
    Home    UMETA(DisplayName = "Home"),
    School  UMETA(DisplayName = "School")
};


UCLASS(Blueprintable)
class ICANTCRY_API UEncounterManager : public UObject
{
    GENERATED_BODY()

public:
    

    // Inizializza il sistema di incontri casuali
    UFUNCTION(BlueprintCallable, Category = "Encounter")
    void Initialize(); 

    // Riduce il valore di threshold in base al movimento del giocatore
    UFUNCTION(BlueprintCallable, Category = "Encounter")
    void UpdateThreshold();

    UFUNCTION(BlueprintCallable, Category = "Encounter")
    void SetPlayerLocationMultiplier(EPlayerLocation NewLocation);

protected:

    // Costruttore
    UEncounterManager();

    class AICC_Player* PlayerRef; // Reference al Player

    // Soglia iniziale random tra 50 e 200
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
    int32 EncounterThreshold;

    // Valore attuale del threshold
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
    int32 CurrentThreshold;

    // Valore sottratto per ogni passo / metro
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
    int32 StepReductionValue;

    // Moltiplicatore per sprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
    float SprintMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
    float LocationMultiplier;

    // Evento Blueprint per innescare una battaglia
    UFUNCTION(Category = "Encounter")
    void StartBattle();

    UFUNCTION(Category = "Encounter")
    void Reset();

    

	
};
