// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleAssembled.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "WorldPartition/ContentBundle/ContentBundleEditorSubsystemInterface.h"

void UPuzzleAssembled::NativeConstruct()
{
	Super::NativeConstruct();
	Return->OnClicked.AddDynamic(this, & UPuzzleAssembled::DisableMinigame);
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
	RemoveFromParent();
}

void UPuzzleAssembled::Display()
{
	Body->SetText(FText::FromString("You assembled correctly all the pieces and for doing that, you obtained a new blueprint"));
	Body->SetAutoWrapText(true);

	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	if (!Blueprint) return;
	Instance->GetCurrentPlayer()->GetInventoryManager()->AddRecipe(Blueprint->RecipeType);
}

void UPuzzleAssembled::SetMinigameOwner(AChallengeMinigame* Minigame)
{
	MinigameOwner = Minigame;
}
