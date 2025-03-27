// Fill out your copyright notice in the Description page of Project Settings.
#include "MinigameHandler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Blueprint/UserWidget.h"


// Sets default values
AMinigameHandler::AMinigameHandler() : AttackMinigame(nullptr), DefenseMinigame(nullptr) , CurrentMinigameDisplayed(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void AMinigameHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMinigameHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinigameHandler::StartMinigame(const bool& EnableAttack)
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	checkf(Controller, TEXT("Controller is null at AMinigameHandler::StartMinigame"));
	
	//bAttackMinigame = EnableAttack;
	
	if (EnableAttack)
	{
		// start attack minigame by default
		CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, AttackMinigame);
	}
	//otherwise call defence minigame 
	if (!EnableAttack)
	{
		CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, DefenseMinigame);
	}

	if (!CurrentMinigameDisplayed)
	{
		DebugHelper::LogError("Couldn't create minigame, something is null");
		return;
	}

	// after that the last thing to do is simply add to viewport and enable ui input

	CurrentMinigameDisplayed->AddToViewport();
	FInputModeUIOnly InputMode;
	Controller->SetInputMode(InputMode);
	Controller->bShowMouseCursor = false;
}

void AMinigameHandler::EndMinigame()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	checkf(Controller, TEXT("Controller is null at AMinigameHandler::StartMinigame"));
	checkf(CurrentMinigameDisplayed, TEXT("Current minigame displayed is null at AMinigameHandler::EndMinigame"));

	// just remove the current minigame from the viewport and make it null so it will be ready to display the new one
	// it disables also

	CurrentMinigameDisplayed->RemoveFromParent();
	CurrentMinigameDisplayed = nullptr;

	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);
	Controller->bShowMouseCursor = false;
}


