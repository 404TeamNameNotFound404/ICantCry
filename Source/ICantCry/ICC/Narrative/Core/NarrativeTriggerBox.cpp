// Fill out your copyright notice in the Description page of Project Settings.


#include "NarrativeTriggerBox.h"
#include "Components/BoxComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/UI/BarkWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANarrativeTriggerBox::ANarrativeTriggerBox()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	RootComponent = TriggerZone;

	
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));

}


void ANarrativeTriggerBox::BeginPlay()
{
	Super::BeginPlay();
    TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ANarrativeTriggerBox::OnOverlapBegin);
    TriggerZone->OnComponentEndOverlap.AddDynamic(this, &ANarrativeTriggerBox::OnOverlapEnd);
}

void ANarrativeTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    AICC_Player* Player = Cast<AICC_Player>(OtherActor);
    if (!Player) return;

    // Se il player è GIÀ dentro (es. secondo componente che overlappa, o si inoltra di più), non rifare nulla
    if (bPlayerInside) return;
    bPlayerInside = true;

    // Trigger once: se è già scattato una volta, non riparte più
    if (bTriggerOnce && bHasBeenTriggered) return;

    if (Player->GetCharacterMovement())
        Player->GetCharacterMovement()->StopMovementImmediately();

    if (DialogueToTrigger && BarkWidgetClass)
    {
        bHasBeenTriggered = true;
        if (UBarkWidget* Bark = CreateWidget<UBarkWidget>(GetWorld(), BarkWidgetClass))
        {
            Bark->AddToViewport();
            Bark->StartBark(DialogueToTrigger);
        }
    }
}


void ANarrativeTriggerBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!Cast<AICC_Player>(OtherActor)) return;

    // Sblocca solo quando NESSUN componente del player overlappa più la box
    // (evita falsi "uscito" se il player ha più collisioni)
    if (!TriggerZone->IsOverlappingActor(OtherActor))
    {
        bPlayerInside = false;
    }
}

