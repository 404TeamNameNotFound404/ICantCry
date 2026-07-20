// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"   

#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/UI/DistantCircleWidget.h"
#include "ICantCry/ICC/Narrative/UI/InteractPromptWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/DataAssets/ICC_InputDataAsset.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"
#include "ICantCry/ICC/Mechanics/Pickups/BasePickup.h"


UInteractionComponent::UInteractionComponent() 
{ 
	PrimaryComponentTick.bCanEverTick = false; 
    bWantsInitializeComponent = true;
}

void UInteractionComponent::InitializeComponent()
{
    //Super::InitializeComponent();

    //if (AActor* Owner = GetOwner())
    //{
    //    // Creiamo dinamicamente il Widget Component 3D e lo attacchiamo all'Actor
    //    InteractionWidgetComponent = NewObject<UWidgetComponent>(Owner, TEXT("InteractionWidget3D"));
    //    if (InteractionWidgetComponent)
    //    {
    //        InteractionWidgetComponent->SetupAttachment(Owner->GetRootComponent());
    //        InteractionWidgetComponent->RegisterComponent();

    //        // Configurazioni per renderlo visibile attraverso i muri (come fanno in TLOU)
    //        InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Resta piatto e guarda la camera
    //        InteractionWidgetComponent->SetDrawSize(FVector2D(200.f, 100.f)); // Dimensioni base
    //        InteractionWidgetComponent->SetVisibility(false); // Inizialmente nascosto
    //    }
    //}

    Super::InitializeComponent();

    if (AActor* Owner = GetOwner())
    {
        InteractionWidgetComponent = NewObject<UWidgetComponent>(Owner, TEXT("InteractionWidget3D"));
        if (InteractionWidgetComponent)
        {

            InteractionWidgetComponent->SetupAttachment(Owner->GetRootComponent());
            InteractionWidgetComponent->RegisterComponent();

            // --- APPLICA OFFSET ---
            InteractionWidgetComponent->SetRelativeLocation(WidgetOffset);

            InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
            InteractionWidgetComponent->SetDrawSize(FVector2D(200.f, 100.f));
            InteractionWidgetComponent->SetVisibility(false);
            InteractionWidgetComponent->SetWindowFocusable(false);
            InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            InteractionWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
        }
    }
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Creiamo in anticipo i due widget se le classi sono state assegnate
    if (DistantCircleWidgetClass)
    {
        DistantCircleWidgetInstance = CreateWidget<UDistantCircleWidget>(GetWorld(), DistantCircleWidgetClass);
    }

    if (InteractPromptWidgetClass)
    {
        InteractPromptWidgetInstance = CreateWidget<UInteractPromptWidget>(GetWorld(), InteractPromptWidgetClass);
    }

    if (!InteractionWidgetComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractionComponent: InteractionWidgetComponent non assegnato per %s"), *GetOwner()->GetName());
    }

   
}


bool UInteractionComponent::CanInteract() const
{
    if (InteractableType == EInteractableType::NPC)
    {
        return true;
    }
    return !bAlreadyRead;
}

void UInteractionComponent::ShowDistantCircleIcon()
{

    if (CurrentWidget == ECurrentWidgetType::DistantCircle) return;

    if (!CanInteract() || !InteractionWidgetComponent || !DistantCircleWidgetInstance) return;

    //ResetPlayerInput();

   /* if (AICC_Player* Player = Cast<AICC_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->SetInputModeToGameAndUI(); 
    }*/

    // Spegniamo l'icona del prompt se era attiva
    if (DistantCircleWidgetInstance)
    {
        DistantCircleWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    // "Iniettiamo" il cerchio distante nel componente 3D
    InteractionWidgetComponent->SetWidget(DistantCircleWidgetInstance);
    InteractionWidgetComponent->SetVisibility(true);

    //DistantCircleWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    DistantCircleWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
    DistantCircleWidgetInstance->SetIconActive(true);


    CurrentWidget = ECurrentWidgetType::DistantCircle;
 
}

void UInteractionComponent::ShowInteractButtonPrompt()
{

    if (CurrentWidget == ECurrentWidgetType::InteractPrompt) return;


    if (!CanInteract() || !InteractionWidgetComponent || !InteractPromptWidgetInstance) return;

    //ResetPlayerInput();


   /* if (AICC_Player* Player = Cast<AICC_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->SetInputModeToGameAndUI();
    }*/

    // Spegniamo il cerchio
    if (DistantCircleWidgetInstance)
    {
        DistantCircleWidgetInstance->SetIconActive(false);
        DistantCircleWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    // --- INIZIO: LOGICA DEL TESTO DINAMICO E ENHANCED INPUT ---
    FKey BoundKey = EKeys::E;
    FText PromptText = FText::FromString(TEXT("Interact"));

    switch (InteractableType)
    {
    case EInteractableType::NPC: PromptText = FText::FromString(TEXT("Speaks")); break;
    case EInteractableType::ItemWithNote: PromptText = FText::FromString(TEXT("Read")); break;
    case EInteractableType::Item: PromptText = FText::FromString(TEXT("Collect")); break;
    case EInteractableType::Inspectable: PromptText = FText::FromString(TEXT("Search")); break;
    default: break;
    }

    if (AICC_Player* Player = Cast<AICC_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        if (Player->GetInputDataAsset())
        {
            if (UInputAction* InteractAction = Player->GetInputDataAsset()->FindNativeInputByTag(Icc_InputTags::InputTag_Interact))
            {
                if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
                {
                    if (ULocalPlayer* LP = PC->GetLocalPlayer())
                    {
                        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
                        {
                            TArray<FKey> MappedKeys = Subsystem->QueryKeysMappedToAction(InteractAction);
                            if (MappedKeys.Num() > 0) BoundKey = MappedKeys[0];
                        }
                    }
                }
            }
        }
    }
    // --- FINE: LOGICA DEL TESTO DINAMICO ---

    // Aggiorniamo i dati grafici
    InteractPromptWidgetInstance->SetPromptData(BoundKey, PromptText);


    // "Iniettiamo" il prompt nel componente 3D
    InteractionWidgetComponent->SetWidget(InteractPromptWidgetInstance);
    InteractionWidgetComponent->SetVisibility(true);

    //InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Visible);
    InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);

    OnInteractPromptShown(InteractPromptWidgetInstance, BoundKey.GetDisplayName());


    CurrentWidget = ECurrentWidgetType::InteractPrompt;


}

void UInteractionComponent::HideAllInteractionUI()
{
    if (InteractionWidgetComponent)
    {
        InteractionWidgetComponent->SetVisibility(false);
    }

    if (DistantCircleWidgetInstance)
    {
        DistantCircleWidgetInstance->SetIconActive(false);
        DistantCircleWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (InteractPromptWidgetInstance)
    {
        InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }

    CurrentWidget = ECurrentWidgetType::None;
   
}

void UInteractionComponent::TriggerInteraction(AICC_Player* Player)
{

 //  // grab the quest manager from game instance, it's a subsystem so this is safe
 //   UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
 //   
	//// start with null instead of default dialogue
 //   // if dialogue to play is still null after the loop, the npc does nothing
 //   UDialogueAsset* DialogueToPlay = nullptr; 
 //   bool bOptional = false;

 //   if (QM)
 //   {
 //       bool bAllQuestsFinished = true;

 //       // iterate through the chain in order, stopping at the first quest that isn't permanently completed
 //       for (const FQuestDialogueChain& Step : QuestChain)
 //       {
 //           if (!Step.Quest) continue;
 //           FGameplayTag QID = Step.Quest->QuestID;

 //           // if the quest is already completed and turned in, skip to the next chain link
	//		// this lets us have multiple quests from the same npc in sequence
 //           if (QM->IsQuestCompleted(QID))
 //           {
 //               continue; 
 //           }

 //           // we found the current quest (the one the player is stuck on or can start)
 //           bAllQuestsFinished = false;

 //           if (QM->IsQuestActive(QID))
 //           {
 //               // quest is active, check if objectives are done to determine if it's turn-in time
 //               if (QM->AreObjectivesCompleteByTag(QID)) 
 //               {
 //                   DialogueToPlay = Step.CompletedDialogue;
 //               }
 //               else 
 //               {
 //                   DialogueToPlay = Step.InProgressDialogue;
 //               }
 //           }
 //           else
 //           {
 //               // quest hasn't been started yet, use the start dialogue
 //               DialogueToPlay = Step.StartDialogue;
 //               bOptional = Step.bIsOptional;
 //           }

 //           // found our current quest, stop iterating so we don't accidentally pick a later one
 //           break; 
 //       }

 //       // if we went through the whole chain and all quests are completed...
 //       if (bAllQuestsFinished)
 //       {
 //           // ...use the final "dumb" dialogue the designers set up
 //           DialogueToPlay = FinalDefaultDialogue;
 //       }
 //   }

 //   // only show the widget if we actually have dialogue to play
 //   // if null (because final default dialogue is empty), the player presses the button but nothing happens
	//// this is intentional so designers can have npcs that become silent after all quests are done
 //   if (DialogueToPlay)
 //   {
 //       UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
 //       if (DialogueWidget)
 //       {
 //           DialogueWidget->AddToViewport();
 //           DialogueWidget->SetIsOptional(bOptional);
 //           DialogueWidget->StartDialogue(DialogueToPlay);
 //       }
 //   }
 //   else 
 //   {
 //       // optional: log or play a small sound feedback if you want
	//	// this helps with debugging so you know the npc was interacted with but had nothing to say
 //       UE_LOG(LogTemp, Log, TEXT("npc has nothing left to say"));
 //   }


    if (!CanInteract() || !Player) return;

    if (InteractableType == EInteractableType::NPC)
    {
        UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
        UDialogueAsset* DialogueToPlay = nullptr;
        bool bOptional = false;

        if (QM)
        {
            bool bAllQuestsFinished = true;

            for (const FQuestDialogueChain& Step : QuestChain)
            {
                if (!Step.Quest) continue;

                // Se la missione è già permanentemente completata, passa alla successiva nella catena
                if (QM->IsQuestCompleted(Step.Quest->QuestID))
                {
                    continue;
                }

                bAllQuestsFinished = false;

                // Se la missione è attiva, controlliamo lo stato degli obiettivi
                if (QM->IsQuestActive(Step.Quest->QuestID))
                {
                    if (QM->AreObjectivesCompleteByTag(Step.Quest->QuestID))
                    {
                        DialogueToPlay = Step.CompletedDialogue;
                    }
                    else
                    {
                        DialogueToPlay = Step.InProgressDialogue;
                    }
                    bOptional = false;
                }
                else
                {
                    // La missione non è ancora iniziata
                    DialogueToPlay = Step.StartDialogue;
                    bOptional = Step.bIsOptional;
                }

                break; // Trovata la missione attuale, interrompe il ciclo sulla catena
            }

            if (bAllQuestsFinished)
            {
                DialogueToPlay = FinalDefaultDialogue;
            }
        }

        if (DialogueToPlay)
        {
            UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
            if (DialogueWidget)
            {
                DialogueWidget->AddToViewport();
                DialogueWidget->SetIsOptional(bOptional);
                DialogueWidget->StartDialogue(DialogueToPlay);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("NPC has nothing left to say"));
        }
    }
    else
    {
        // Interazione di tipo Nota o Oggetto Fisico
        HandleCollectibleCollection(Player);
    }


}

void UInteractionComponent::HandleCollectibleCollection(AICC_Player* Player)
{
    //if (!Player) return;

    //bAlreadyRead = true;
    //HideAllInteractionUI();

    //// Controlliamo se questo Actor è un "Raccoglibile" (BasePickup)
    //ABasePickup* PickupOwner = Cast<ABasePickup>(GetOwner());

    //switch (InteractableType)
    //{
    //case EInteractableType::Item:
    //    // Sparisce + Bark subito. La quest avanza in BasePickup::Collect (tag del pickup).
    //    Player->PlayBarkImmediately(PostPickupBark);
    //    if (PickupOwner) PickupOwner->Collect(Player);
    //    else if (AActor* Owner = GetOwner()) Owner->Destroy();
    //    break;

    //case EInteractableType::ItemWithNote:
    //    // Sparisce + apre Bestiario sulla entry; il Bark parte alla CHIUSURA del Bestiario.
    //    Player->OpenBestiaryOnEntryAndListenForClose(BestiaryEntryID, PostPickupBark);
    //    if (PickupOwner) PickupOwner->Collect(Player);
    //    else if (AActor* Owner = GetOwner()) Owner->Destroy();
    //    break;

    //case EInteractableType::Inspectable:
    //    // Resta nel mondo + Bark subito + avanza quest (tag del componente).
    //    Player->PlayBarkImmediately(PostPickupBark);
    //    if (UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>())
    //    {
    //        if (QuestTagToUpdate.IsValid() && ObjectiveTagToUpdate.IsValid())
    //            QM->UpdateObjectiveProgress(QuestTagToUpdate, ObjectiveTagToUpdate, ProgressAmount);
    //    }
    //    Deactivate();
    //    break;

    //default:
    //    break;
    //}


    if (!Player) return;

    HideAllInteractionUI();

    ABasePickup* PickupOwner = Cast<ABasePickup>(GetOwner());
    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();

    switch (InteractableType)
    {
    case EInteractableType::Item:
        // Sparisce + feedback subito. La quest avanza in BasePickup::Collect (tag del pickup).
        bAlreadyRead = true;
        PlayPrimaryFeedback(Player);
        if (PickupOwner) PickupOwner->Collect(Player);
        else if (AActor* Owner = GetOwner()) Owner->Destroy();
        break;

    case EInteractableType::ItemWithNote:
        // Sparisce + apre Bestiario sulla entry; il Bark parte alla CHIUSURA del Bestiario.
        bAlreadyRead = true;
        Player->OpenBestiaryOnEntryAndListenForClose(BestiaryEntryID, PostPickupBark);
        if (PickupOwner) PickupOwner->Collect(Player);
        else if (AActor* Owner = GetOwner()) Owner->Destroy();
        break;

    case EInteractableType::Inspectable:
    {
        const bool bHasQuestToUpdate = QuestTagToUpdate.IsValid() && ObjectiveTagToUpdate.IsValid();

        if (bRepeatable)
        {
            if (!bInspectableUsed)
            {
                // Prima ispezione: avanza la quest (se impostata) + feedback principale.
                if (bHasQuestToUpdate && QM)
                    QM->UpdateObjectiveProgress(QuestTagToUpdate, ObjectiveTagToUpdate, ProgressAmount);

                PlayPrimaryFeedback(Player);
                bInspectableUsed = true;
            }
            else
            {
                // Ispezioni successive: solo bark di default, nessun avanzamento quest.
                Player->PlayBarkImmediately(FinalDefaultBark);
            }
            // Ripetibile: NON disabilitiamo (bAlreadyRead resta false, niente Deactivate).
        }
        else
        {
            // One-shot classico.
            if (bHasQuestToUpdate && QM)
            {
                QM->UpdateObjectiveProgress(QuestTagToUpdate, ObjectiveTagToUpdate, ProgressAmount);
                PlayPrimaryFeedback(Player);
            }
            else
            {
                // Nessuna quest da aggiornare: fallback sul bark di default (o sul feedback principale se assente).
                if (FinalDefaultBark) Player->PlayBarkImmediately(FinalDefaultBark);
                else PlayPrimaryFeedback(Player);
            }

            bAlreadyRead = true;
            Deactivate();
        }
        break;
    }

    default:
        break;
    }
    
}


void UInteractionComponent::PlayPrimaryFeedback(AICC_Player* Player)
{
    if (!Player) return;

    if (PostPickupFeedbackType == EPostPickupFeedback::DialogueUI)
    {
        Player->PlayDialogueUIImmediately(PostPickupDialogue);
    }
    else
    {
        Player->PlayBarkImmediately(PostPickupBark);
    }
}


