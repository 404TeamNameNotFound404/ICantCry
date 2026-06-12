// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_StartDialogue.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "Blueprint/UserWidget.h"

void UEvent_StartDialogue::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	if (!Player || !WidgetClass || !DialogueToPlay) return;

	auto DialogueWidget = CreateWidget<UDialogueWidget>(Player->GetWorld(), WidgetClass);
	if (DialogueWidget)
	{
		DialogueWidget->AddToViewport();
		DialogueWidget->StartDialogue(DialogueToPlay);

		// lock player movement and focus ui for convo
		if (auto PC = Player->GetController<APlayerController>())
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}