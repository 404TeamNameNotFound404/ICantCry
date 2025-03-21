// Fill out your copyright notice in the Description page of Project Settings.


#include "EncounterManager.h"

UEncounterManager::UEncounterManager(){}

void UEncounterManager::Initialize()
{
    EncounterThreshold = FMath::RandRange(50, 200); 
    CurrentThreshold = EncounterThreshold; 
    SprintMultiplier = 1.5f; 
    LocationMultiplier = 1.0f; // Default: no multiplier activeted

    
}
void UEncounterManager::UpdateThreshold()
{
    if (!PlayerRef) return;

    StepReductionValue = PlayerRef->GetStepCounter();

    float ReductionValue = (PlayerRef->IsSprinting() ?  (SprintMultiplier * GetWorld()->GetDeltaSeconds()) / StepReductionValue : StepReductionValue) * LocationMultiplier * GetWorld()->GetDeltaSeconds() ;

    CurrentThreshold -= ReductionValue;

    UE_LOG(LogTemp, Warning, TEXT("CurrentThreshold: %d"), CurrentThreshold);

    if (CurrentThreshold <= 0)
    {
        StartBattle();   
    }
}



void UEncounterManager::SetPlayerLocationMultiplier(EPlayerLocation NewLocation)
{
    switch (NewLocation)
    {
    case EPlayerLocation::Home:
        LocationMultiplier = 0.1f;
        break;
    case EPlayerLocation::School:
        LocationMultiplier = 2.5f;
        break;
    default:
        LocationMultiplier = 1.0f; // default
        break;
    }

    //UE_LOG(LogTemp, Warning, TEXT("Location Multiplier update to: %f"), LocationMultiplier);

    UE_LOG(LogTemp, Warning, TEXT("The player has entered %s - Multiplier: %f"),
    *UEnum::GetValueAsString(NewLocation), LocationMultiplier);

    // Whene player change zone, call 
    //EncounterManager->SetPlayerLocationMultiplier(EPlayerLocation::Home); 
    //EncounterManager->SetPlayerLocationMultiplier(EPlayerLocation::School); 
}


void UEncounterManager::StartBattle()
{
    UE_LOG(LogTemp, Warning, TEXT("Start Battle!"));

}

void UEncounterManager::Reset()
{   
    EncounterThreshold = FMath::RandRange(50, 200);
}