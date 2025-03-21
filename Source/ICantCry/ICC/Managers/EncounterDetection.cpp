// Fill out your copyright notice in the Description page of Project Settings.


#include "EncounterDetection.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
AEncounterDetection::AEncounterDetection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Home"));

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AEncounterDetection::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AEncounterDetection::BeginPlay()
{
	Super::BeginPlay();
	
	EncounterManager = NewObject<UEncounterManager>();
	EncounterManager->Initialize();
}

// Called every frame
void AEncounterDetection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEncounterDetection::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (!Player || OtherActor != Player)
    {
        DebugHelper::LogError("the player is not found");

        return;
    }
	
	if(bIsSchool)
	{
		EncounterManager->SetPlayerLocationMultiplier(EPlayerLocation::School);
		DebugHelper::LogMessage(15,FColor::Green,"the player has entered the SCHOOL zone");
	}
	else
	{
		EncounterManager->SetPlayerLocationMultiplier(EPlayerLocation::Home);
		DebugHelper::LogMessage(15,FColor::Green,"the player has entered the HOME zone");
	}
	

	
}

void AEncounterDetection::OnOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	bIsSchool = false;
}
