// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_StartDialogue.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "Blueprint/UserWidget.h"

void UEvent_StartDialogue::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	if (!Player || !WidgetClass || !DialogueToPlay) return;

	// Creiamo l'istanza del Widget nel mondo del Player
	UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(Player->GetWorld(), WidgetClass);
	
	if (DialogueWidget)
	{
		DialogueWidget->AddToViewport();
		
		// Passiamo i dati al Widget e facciamolo partire
		DialogueWidget->StartDialogue(DialogueToPlay);

		// Opzionale: Blocchiamo l'input del giocatore durante il dialogo
		APlayerController* PC = Player->GetController<APlayerController>();
		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}