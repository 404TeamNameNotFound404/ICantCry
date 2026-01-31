// Fill out your copyright notice in the Description page of Project Settings.
#include "Paper.h"
#include "ICantCry/ICC/Mechanics/ChallengeMinigames/ChallengeMinigame.h"


// Sets default values
APaper::APaper()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Challenge"));
	PaperMesh = CreateDefaultSubobject<UStaticMeshComponent>("PaperMesh");
}

// Called when the game starts or when spawned
void APaper::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	CurrentLocation = StartLocation;

	if (Texture)
	{
		SetTexture(Texture);
	}
}

// Called every frame
void APaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBeingDragged)
	{
		if (FVector::Dist(CurrentLocation, StartLocation) < SnapDistance)
		{
			return;
		}

		if (const AFieldSlot* Slot = GetCurrentSlot();
			const AFieldSlot* ClosestSlot = Minigame->FindSlot(CurrentLocation))
		{
			if (ClosestSlot != Slot &&
				FVector::Dist(CurrentLocation, ClosestSlot->GetActorLocation()) < Minigame->GetSnapDistance())
			{
				SetActorLocation(ClosestSlot->GetActorLocation());
			}
		}
	}


}

void APaper::Lift(const FVector& LiftLocation)
{
	SetActorLocation(LiftLocation);
	CurrentLocation = LiftLocation;
	bLifted = true;
	bIsSwapped = false;
	bIsBeingDragged = true;
}

void APaper::Release()
{
	// SetActorLocation(StartLocation);
	bIsBeingDragged = false;
	bLifted = false;
	bIsSwapped = false;
	Minigame->ReleasePaper();
}

FVector APaper::GetStartLocation() const
{
	return StartLocation;
}

FVector APaper::GetCurrentLocation() const
{
	return CurrentLocation;
}

bool APaper::GetIsLifted() const
{
	return bLifted;
}

bool APaper::GetIsSwapped() const
{
	return bIsSwapped;
}


void APaper::SetIsDragged(const bool& Value)
{
	bIsBeingDragged = Value;
}

void APaper::SetMinigameOwner(AChallengeMinigame* Value)
{
	Minigame = Value;
}

void APaper::SetCurrentSlot(AFieldSlot* Value)
{
	CurrentSlot = Value;
}

AFieldSlot* APaper::GetCurrentSlot() const
{
	return CurrentSlot;
}

void APaper::SetTexture(UTexture2D* Value)
{
	if (PaperMesh && Value)
	{
		if (!PaperMaterial)
		{
			PaperMaterial = UMaterialInstanceDynamic::Create(PaperMesh->GetMaterial(0), this);
			PaperMesh->SetMaterial(0, PaperMaterial);
		}
		
		PaperMaterial->SetTextureParameterValue("PaperTexture", Value); 
	}
}

UTexture2D* APaper::GetTexture() const
{
	return Texture;
}

int32 APaper::GetId() const
{
	return Id;
}
