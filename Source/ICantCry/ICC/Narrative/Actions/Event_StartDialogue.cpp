// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_StartDialogue.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/UI/BarkWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Blueprint/UserWidget.h"

void UEvent_StartDialogue::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	//if (!Player || !WidgetClass || !DialogueToPlay) return;

	//auto DialogueWidget = CreateWidget<UDialogueWidget>(Player->GetWorld(), WidgetClass);
	//if (DialogueWidget)
	//{
	//	DialogueWidget->AddToViewport();
	//	DialogueWidget->StartDialogue(DialogueToPlay);

	//	// lock player movement and focus ui for convo
	//	if (auto PC = Player->GetController<APlayerController>())
	//	{
	//		FInputModeGameAndUI InputMode;
	//		InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
	//		PC->SetInputMode(InputMode);
	//		PC->bShowMouseCursor = true;
	//	}
	//}


    if (!Player || !WidgetClass || !DialogueToPlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_StartDialogue: Player, WidgetClass o DialogueToPlay non impostati."));
        return;
    }

    UUserWidget* NewWidget = CreateWidget<UUserWidget>(Player->GetWorld(), WidgetClass);
    if (!NewWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_StartDialogue: creazione del widget fallita."));
        return;
    }

    NewWidget->AddToViewport();

    // Sceglia il metodo giusto in base al tipo reale del widget scelto dal designer.
    if (UDialogueWidget* AsDialogue = Cast<UDialogueWidget>(NewWidget))
    {
        AsDialogue->StartDialogue(DialogueToPlay);
    }
    else if (UBarkWidget* AsBark = Cast<UBarkWidget>(NewWidget))
    {
        AsBark->StartBark(DialogueToPlay);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Event_StartDialogue: WidgetClass non è né UDialogueWidget né UBarkWidget."));
        NewWidget->RemoveFromParent();
        return;
    }
}