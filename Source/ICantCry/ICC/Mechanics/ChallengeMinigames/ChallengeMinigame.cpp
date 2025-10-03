// Fill out your copyright notice in the Description page of Project Settings.


#include "ChallengeMinigame.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


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

	DebugHelper::LogWarning("Blueprint suppose to be visible");
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Visible);
}

void AChallengeMinigame::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (OtherActor != Cast<AActor>(Instance->GetCurrentPlayer()))
	{
		return;
	}

	bInArea = false;
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden);
}

// Called every frame
void AChallengeMinigame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bInArea /*|| (bInArea && !bIsMinigameStarted)*/) // Note: this will be enabled later (waiting for Simone)
	{
		return;
	}

	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->SetViewTargetWithBlend(CameraActor, 0.5f);
	TriggerWidgetBlueprint->SetVisibility(ESlateVisibility::Hidden); // Note: Remove this later and add it when player press the correct button

	Instance->GetCurrentPlayer()->DisableInput(Controller);
	
}

bool AChallengeMinigame::GetIsInArea() const
{
	return bInArea;
}

void AChallengeMinigame::Exit()
{
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->SetViewTargetWithBlend(Instance->GetCurrentPlayer(), 0.5f);
	Instance->GetCurrentPlayer()->EnableInput(Controller);
}

