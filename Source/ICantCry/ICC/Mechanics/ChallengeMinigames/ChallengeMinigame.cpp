// Fill out your copyright notice in the Description page of Project Settings.


#include "ChallengeMinigame.h"
#include "PuzzleAssembled.h"
#include "Algo/RandomShuffle.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Actors/MinigameSpawnables/Papers/Paper.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "PuzzleAssembled.h"
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

	DebugHelper::LogWarning("Slot size: " + FString::FromInt(TerrainSlots.Num()));
}

void AChallengeMinigame::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != Cast<AActor>(Instance->GetCurrentPlayer()) || bInArea)
	{
		return;
	}

	bInArea = true;
	bIsMinigameStarted = true;
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
	
	for (int i = 0; i < TerrainSlots.Num(); ++i)
	{
		auto Slot = TerrainSlots[i];
		if (!Slot)
		{
			DebugHelper::LogError("Slot is nullptr!");
			continue;
		}

		const FVector SpawnLocation = Slot->GetActorLocation();
		
		TSubclassOf<APaper> ChosenPaperClass = Papers[i % Papers.Num()]; 
		
		if (APaper* Paper = GetWorld()->SpawnActor<APaper>(ChosenPaperClass, SpawnLocation + LocationOffset, Slot->GetActorRotation()); Paper)
		{
			Paper->SetActorLabel("Paper " + FString::FromInt(i));
			Paper->SetMinigameOwner(this);
			Paper->SetCurrentSlot(Slot);
			PaperMap.Add(Slot, Paper);

			DebugHelper::LogMessage(8, FColor::FromHex("434E78"), Paper->GetName() + " id is " + FString::FromInt(Paper->GetId()));
			DebugHelper::LogMessage(8, FColor::FromHex("434E78"), Slot->GetName() + " id is " + FString::FromInt(Slot->GetId()));
		}
	}
	
	DebugHelper::LogSuccess("Papers spawned");
	DebugHelper::LogSuccess("Papers in map: " + FString::FromInt(PaperMap.Num()));
}

void AChallengeMinigame::Resolve()
{
	bool bSolutionFound = true;

	for (AFieldSlot* Slot : TerrainSlots)
	{
		if (!Slot) continue;

		APaper* const* PaperPtr = PaperMap.Find(Slot);
		if (!PaperPtr || !*PaperPtr)
		{
			bSolutionFound = false;
			DebugHelper::LogError("No paper in slot " + Slot->GetName());
			continue;
		}

		const APaper* Paper = *PaperPtr;

		if (Paper->GetId() == Slot->GetId())
		{
			DebugHelper::LogSuccess(Slot->GetName() + " correctly occupied by paper " + FString::FromInt(Paper->GetId()));
		}
		else
		{
			bSolutionFound = false;
			DebugHelper::LogError("Paper " + FString::FromInt(Paper->GetId()) +
				" is in wrong slot " + FString::FromInt(Slot->GetId()));
		}
	}

	if (bSolutionFound)
	{
		DebugHelper::LogSuccess("All papers are correctly placed!");
		UPuzzleAssembled* Victory = CreateWidget<UPuzzleAssembled>(GetWorld(), VictoryWidgetClass);
		Victory->SetMinigameOwner(this);
		Victory->AddToViewport();
		Victory->Display();
		Victory->SetVisibility(ESlateVisibility::Visible);
	}
}


void AChallengeMinigame::PickPaper()
{
	if (!bInArea) // prevent to try to pick any papers on the game (T.T)
	{
		return;
	}
		
	FHitResult Hit;
	const AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!Hit.bBlockingHit)
	{
		return;
	}
	
	APaper* HitActor = Cast<APaper>(Hit.GetActor());
	
	if (!HitActor || !HitActor->Tags.Contains("Challenge"))
	{
		DebugHelper::LogMessage(5, FColor::Orange, "Paper is invalid dafuk");
		return;
	}
	
	
	if (FVector WorldLocation, WorldDirection; Controller->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
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
}

APaper* AChallengeMinigame::GetCurrentPaper() const
{
	return CurrentPaper;
}

TArray<TObjectPtr<AFieldSlot>> AChallengeMinigame::GetFieldSlots() const
{
	return TerrainSlots;
}

void AChallengeMinigame::PlacePaperInSlot(APaper* Paper, AFieldSlot* Slot)
{
	if (!Paper || !Slot) return;

	AFieldSlot* OldSlot = Paper->GetCurrentSlot();
	
	if (OldSlot && OldSlot != Slot)
	{
		PaperMap.Remove(OldSlot);
	}
	
	if (APaper* OverlappedPaper = PaperMap.FindRef(Slot); OverlappedPaper && OverlappedPaper != Paper)
	{
		if (OldSlot)
		{
			OverlappedPaper->SetCurrentSlot(OldSlot);
			OverlappedPaper->SetActorLocation(OldSlot->GetActorLocation() + FVector{0, 0, 1.0f});
			PaperMap.Add(OldSlot, OverlappedPaper);
		}
		else
		{
			OverlappedPaper->SetCurrentSlot(nullptr);
			OverlappedPaper->SetActorLocation(OverlappedPaper->GetStartLocation());
		}
	}

	Paper->SetCurrentSlot(Slot);
	Paper->SetActorLocation(Slot->GetActorLocation() + FVector{0,0,1.5f});
	PaperMap.Add(Slot, Paper);
	DebugHelper::LogSuccess("Paper swapped");
}

AFieldSlot* AChallengeMinigame::FindSlot(const FVector& PaperLocation) const
{
	AFieldSlot* Slot = nullptr;
	float Dist = SnapDistance;

	for (AFieldSlot* Field : TerrainSlots)
	{
		if (!Field) continue;
		
		if (const float Distance = FVector::Dist(PaperLocation, Field->GetActorLocation()); Distance < Dist)
		{
			Dist = Distance;
			Slot = Field;
		}
	}

	return Slot;
}

void AChallengeMinigame::ReleasePaper()
{
	if (!CurrentPaper) return;
	
	if (AFieldSlot* ClosestSlot = FindSlot(CurrentPaper->GetActorLocation()); ClosestSlot)
	{
		PlacePaperInSlot(CurrentPaper, ClosestSlot);
	}
	else
	{
		CurrentPaper->SetActorLocation(CurrentPaper->GetStartLocation());
		CurrentPaper->SetCurrentSlot(nullptr);
	}

	CurrentPaper->SetIsDragged(false);
	CurrentPaper = nullptr;
	Resolve();
}

float AChallengeMinigame::GetSnapDistance() const
{
	return SnapDistance;
}

void AChallengeMinigame::SetMinigameStarted(const bool& Value)
{
	bIsMinigameStarted = Value;
}

UBoxComponent* AChallengeMinigame::GetTriggerComponent() const
{
	return Trigger;
}

