// Fill out your copyright notice in the Description page of Project Settings.


#include "ChallengeMinigame.h"

#include "Algo/RandomShuffle.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Actors/MinigameSpawnables/Papers/Paper.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

AChallengeMinigame* AChallengeMinigame::Singleton = nullptr;


// Sets default values
AChallengeMinigame::AChallengeMinigame()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AChallengeMinigame::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AChallengeMinigame::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AChallengeMinigame::BeginPlay()
{
	Super::BeginPlay();

	TriggerWidgetBlueprint = CreateWidget<UUserWidget>(GetWorld(), TriggerWidget);
	checkf(TriggerWidgetBlueprint, TEXT("Blueprint is invalid"))
	TriggerWidgetBlueprint->AddToViewport();
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden);

	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
}

void AChallengeMinigame::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != Cast<AActor>(Instance->GetCurrentPlayer()))
	{
		return;
	}

	bInArea = true;
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->SetViewTargetWithBlend(CameraActor, 0.5f);
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden); 
	Instance->GetCurrentPlayer()->GetCharacterMovement()->DisableMovement();
	Controller->SetShowMouseCursor(true);
	Singleton = this;

	InitSlots();
	DebugHelper::LogWarning("Blueprint suppose to be visible");
	//TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Visible);
}

void AChallengeMinigame::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (OtherActor != Cast<AActor>(Instance->GetCurrentPlayer()))
	{
		return;
	}
	
	bInArea = false;
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	//Instance->GetCurrentPlayer()->EnableInput(Controller);
	Instance->GetCurrentPlayer()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Singleton = nullptr;
	Controller->SetShowMouseCursor(false);
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden);
}

// Called every frame
void AChallengeMinigame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (!bInArea /*|| (bInArea && !bIsMinigameStarted)*/) // Note: this will be enabled later (waiting for Simone)
	// {
	// 	return;
	// }
}

bool AChallengeMinigame::GetIsInArea() const
{
	return bInArea;
}

void AChallengeMinigame::Exit()
{
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->SetViewTargetWithBlend(Instance->GetCurrentPlayer(), 0.5f);
	Instance->GetCurrentPlayer()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Controller->SetShowMouseCursor(false);
}

void AChallengeMinigame::InitSlots()
{
	if (Papers.IsEmpty() || TerrainSlots.IsEmpty())
	{
		return;
	}
	
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden); // For now i re-hide the text ui once it enters this line will go to the input logic

	Algo::RandomShuffle(TerrainSlots);

	for (TSoftObjectPtr<AActor> Slot : TerrainSlots)
	{
		FVector SpawnLocation = Slot->GetActorLocation();
		GetWorld()->SpawnActor<APaper>(Papers[0], SpawnLocation + LocationOffset, Slot->GetActorRotation());
	}
}

void AChallengeMinigame::PickPaper()
{
	FHitResult Hit;
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!Hit.bBlockingHit)
	{
		return;
	}


	
	APaper* HitActor = Cast<APaper>(Hit.GetActor());
	//checkf(HitActor, TEXT("APaper is invalid at AChallengeMinigame::CalculateCursorCameraProjection"))
	
	if (!HitActor || !HitActor->Tags.Contains("Challenge"))
	{
		DebugHelper::LogMessage(5, FColor::Orange, "Paper is invalid dafuk");
		return;
	}
	
	// const FVector LiftLocation = HitActor->GetActorLocation() + FVector{0, 0, 10};
	// HitActor->Lift(LiftLocation);

	FVector WorldLocation, WorldDirection;
	if (Controller->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Trace forward to find ground or target point
		FHitResult GroundHit;
		const FVector TraceStart = WorldLocation;
		const FVector TraceEnd = TraceStart + WorldDirection * 10000.0f;

		if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility))
		{
			const FVector LiftLocation = GroundHit.Location /*+ FVector(0, 0, 3.5)*/;
			HitActor->Lift(LiftLocation);
			CurrentPaper = HitActor;
		}
	}

	// Fallback: lift slightly from current location
	// const FVector FallbackLift = HitActor->GetActorLocation() + FVector(0, 0, 3.5f);
	// HitActor->Lift(FallbackLift);
}

APaper* AChallengeMinigame::GetCurrentPaper() const
{
	return CurrentPaper;
}

