// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleAssembled.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"

void UPuzzleAssembled::NativeConstruct()
{
	Super::NativeConstruct();
	Return->OnClicked.AddDynamic(this, & UPuzzleAssembled::DisableMinigame);

	Self.RequiredBlueprintType = RecipeType;
}

void UPuzzleAssembled::DisableMinigame()
{
	MinigameOwner->SetMinigameStarted(false);
	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Controller->SetViewTargetWithBlend(Instance->GetCurrentPlayer(), 0.5f);
	Instance->GetCurrentPlayer()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Controller->SetShowMouseCursor(false);
	MinigameOwner->GetTriggerComponent()->SetActive(false);
	MinigameOwner->GetTriggerComponent()->DestroyComponent();
	//Instance->GetCurrentPlayer()->GetInventoryManager()->AddRecipe(Self);
	Instance->GetCurrentPlayer()->GetInventoryManager()->RecipeUnlocked.Broadcast(Self.RequiredBlueprintType);
	RemoveFromParent();
}

void UPuzzleAssembled::Display()
{
	Body->SetText(FText::FromString("You obtained " + Self.GetName(RecipeType) + " Blueprint"));
	Body->SetAutoWrapText(true);
}

void UPuzzleAssembled::SetMinigameOwner(AChallengeMinigame* Minigame)
{
	MinigameOwner = Minigame;
}
