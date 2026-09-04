// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_DropItem.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Narrative/Components/InteractionComponent.h"



void UEvent_DropItem::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (!ItemClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_DropItem: ItemClass non impostata, niente da spawnare."));
        return;
    }

    AActor* NPCActor = nullptr;
    if (Player && Player->GetCurrentInteractableTarget())
    {
        NPCActor = Player->GetCurrentInteractableTarget()->GetOwner();
    }
    if (!NPCActor)
    {
        NPCActor = Cast<AActor>(Context);
    }

    AActor* SpawnReference = NPCActor ? NPCActor : Player;
    if (!SpawnReference)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_DropItem: nessun riferimento valido (ne' NPC ne' Player). Drop annullato."));
        return;
    }

    UWorld* World = SpawnReference->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_DropItem: World nullo. Drop annullato."));
        return;
    }

    
    const FVector ForwardDir = SpawnReference->GetActorForwardVector();
    const FVector ProvisionalLocation = SpawnReference->GetActorLocation() + (ForwardDir * SpawnForwardOffset);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* SpawnedItem = World->SpawnActor<AActor>(ItemClass, ProvisionalLocation, FRotator::ZeroRotator, SpawnParams);
    if (!SpawnedItem)
    {
        UE_LOG(LogTemp, Error, TEXT("Event_DropItem: SpawnActor fallito per la classe %s."), *ItemClass->GetName());
        return;
    }

    
    FVector ItemOrigin;
    FVector ItemExtent;
    SpawnedItem->GetActorBounds(false, ItemOrigin, ItemExtent);

    
    const FVector TraceStart = ProvisionalLocation + FVector(0.0f, 0.0f, 100.0f);
    const FVector TraceEnd = ProvisionalLocation - FVector(0.0f, 0.0f, GroundTraceDistance);

    
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(SpawnedItem);
    if (NPCActor) TraceParams.AddIgnoredActor(NPCActor);
    if (Player)   TraceParams.AddIgnoredActor(Player);

    FHitResult GroundHit;
   
    const bool bHitGround = World->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams);

    if (bHitGround)
    {
       
        const FVector RestingLocation(ProvisionalLocation.X, ProvisionalLocation.Y, GroundHit.ImpactPoint.Z + ItemExtent.Z);
        SpawnedItem->SetActorLocation(RestingLocation);
    }
    else
    {
       
        UE_LOG(LogTemp, Warning, TEXT("Event_DropItem: nessun terreno trovato sotto lo spawn di %s."), *SpawnedItem->GetName());
    }

    
    if (!Player)
    {
        return;
    }

    const float ItemHorizontalRadius = FMath::Max(ItemExtent.X, ItemExtent.Y);
    const float PushDistance = (2.0f * ItemHorizontalRadius) + ExtraClearance;

    
    FVector PushDir = NPCActor
        ? (Player->GetActorLocation() - NPCActor->GetActorLocation())
        : -Player->GetActorForwardVector();
    PushDir.Z = 0.0f; 

    if (!PushDir.Normalize())
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_DropItem: direzione di spinta nulla, player non spostato."));
        return;
    }

    const FVector NewPlayerLocation = Player->GetActorLocation() + (PushDir * PushDistance);
    Player->SetActorLocation(NewPlayerLocation, true);


}