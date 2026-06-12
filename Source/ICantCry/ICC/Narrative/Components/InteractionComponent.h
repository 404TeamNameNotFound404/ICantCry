// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"
#include "InteractionComponent.generated.h"


class UDialogueAsset;
class UQuestDefinition;
class UDialogueWidget;
class AICC_Player;
class UICantCryGameInstance;
class UDistantCircleWidget;
class UInteractPromptWidget;
class UICC_InputDataAsset;
class UInteractPromptWidget;
class ABasePickup;
class UWidgetInteractionComponent;



UENUM(BlueprintType)
enum class EInteractableType : uint8
{
    NPC          UMETA(DisplayName = "NPC / Dialogue"),
    ItemWithNote UMETA(DisplayName = "ItemWithNote: Disappears, opens Bestiary, plays Bark on Bestiary close if set"),
    Item         UMETA(DisplayName = "Item:Disappears, plays Bark, advances Quest if set"),
    Inspectable  UMETA(DisplayName = "Inspectable :Stays in world, plays Bark, advances Quest if set")
};




USTRUCT(BlueprintType)
struct FQuestDialogueChain
{
    GENERATED_BODY()

    /** the quest associated with this chain link, we use its tag to check status in the quest manager */
    UPROPERTY(EditAnywhere, Category = "Quest")
    TObjectPtr<UQuestDefinition> Quest;

    /** if TRUE, the dialogue widget will show accept/decline buttons for this quest, otherwise it starts automatically */
    UPROPERTY(EditAnywhere, Category = "Quest")
    bool bIsOptional = false;

    /** dialogue to play when player meets the npc and this quest hasn't been started yet */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> StartDialogue;

    /** dialogue to play when the quest is active but objectives are not all complete yet */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> InProgressDialogue;

    /** dialogue to play when the quest is active and ALL objectives are complete (ready to turn in) */
    UPROPERTY(EditAnywhere, Category = "Dialogue")
    TObjectPtr<UDialogueAsset> CompletedDialogue;
};



/**
 * CLASS: UInteractionComponent
 * DESCRIPTION: component to attach to any actor that should be interactable (npcs, quest givers, etc)
 * handles the logic of selecting the correct dialogue based on quest states in the quest manager
 * Universal interaction component (NPCs, Notes, Items).
 * Manages both narrative quest chains and the collection system.
 */
UCLASS(ClassGroup=(Narrative), meta=(BlueprintSpawnableComponent), BlueprintType)
class ICANTCRY_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

    virtual void InitializeComponent() override;
  
    // -----BASE CONFIGURATION------

    /** What kind of interaction is this? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    EInteractableType InteractableType = EInteractableType::NPC;
    
    /** Text to display on the screen (e.g. "Read Letter" or "Collect Key") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FText ActionText;

    // --- INPUT MANAGEMENT ---
    /** Data asset reference to dynamically look up input action bindings */
    UPROPERTY(EditDefaultsOnly, Category = "Interaction|Input")
    TObjectPtr<UICC_InputDataAsset> InputDataAsset;

    
    // -----WIDGET CONFIGURATION------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI Config")
    TSubclassOf<class UDistantCircleWidget> DistantCircleWidgetClass;

    /** Classe del Widget per il prompt interattivo ravvicinato (Tasto E + Testo) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI Config")
    TSubclassOf<class UInteractPromptWidget> InteractPromptWidgetClass;

    /** Offset relativo al root dell'attore per posizionare il widget 3D (usa il gizmo nell'editor) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI Config", meta = (MakeEditWidget = true))
    FVector WidgetOffset = FVector(0, 0, 100);


    // -----COLLECTIBLES  CONFIGURATION (NOTES / OBJS)----

    /** ID of the Note to unlock in the GameInstance/Bestiary */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collectibles",
        meta = (EditCondition = "InteractableType == EInteractableType::ItemWithNote", EditConditionHides))
    FString BestiaryEntryID;

    /** Bark — per tutti tranne NPC */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Collectibles",
        meta = (EditCondition = "InteractableType != EInteractableType::NPC", EditConditionHides))
    UDialogueAsset* PostPickupBark;


    // --- QUEST UPDATES (Visible only if it is a Quest Item without a note) ---

    UPROPERTY(EditAnywhere, Category = "Interaction|Quest Update",
        meta = (EditCondition = "InteractableType == EInteractableType::Inspectable", EditConditionHides))
    FGameplayTag QuestTagToUpdate;

    UPROPERTY(EditAnywhere, Category = "Interaction|Quest Update",
        meta = (EditCondition = "InteractableType == EInteractableType::Inspectable", EditConditionHides))
    FGameplayTag ObjectiveTagToUpdate;

    UPROPERTY(EditAnywhere, Category = "Interaction|Quest Update",
        meta = (EditCondition = "InteractableType == EInteractableType::Inspectable", EditConditionHides))
    int32 ProgressAmount = 1;

     
    // ----DIALOGUE CONFIGURATION -----

	/** 
     * ordered list of quests this npc handles. order matters: 
     * the system iterates through the list until it finds the first quest NOT permanently completed
     * and uses that to determine the dialogue. if all are completed, uses FinalDefaultDialogue
     */
    UPROPERTY(EditAnywhere, Category = "Narrative|Config", meta = (EditCondition = "InteractableType == EInteractableType::NPC", EditConditionHides))
    TArray<FQuestDialogueChain> QuestChain;

    /** fallback dialogue when all quests in the chain have been completed and turned in */
    UPROPERTY(EditAnywhere, Category = "Narrative|Dialogue", meta = (EditCondition = "InteractableType == EInteractableType::NPC", EditConditionHides))
    TObjectPtr<UDialogueAsset> FinalDefaultDialogue;


	/** widget class to instantiate when a dialogue starts, must derive from UDialogueWidget */
	UPROPERTY(EditAnywhere, Category = "Narrative|Config", meta = (EditCondition = "InteractableType == EInteractableType::NPC", EditConditionHides))
	TSubclassOf<class UDialogueWidget> DialogueWidgetClass;

    // --- INTERACTION STATE ---

    /** Save if the note/object has already been read/collected to disable future interaction */
    UPROPERTY(BlueprintReadOnly, Category = "Interaction|State")
    bool bAlreadyRead = false;
    
    // --- FUNCTIONS---

    /** entry point called by the player when they press the interact button on this npc */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TriggerInteraction(AICC_Player* Player);

    /** Funzione /** Function used by the Scanner to know if the object is still active or should be ignored */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool CanInteract() const;

   
    // --- WIDGET MANAGEMENT---

    /** Called by the scanner when the object is in general view range (shows the white circle) */
    void ShowDistantCircleIcon();

    /** Called by the scanner when the player approaches and the 0.2s delay has passed (shows the E input key) */
    void ShowInteractButtonPrompt();

    /** Instantly hides any interaction graphics */
    void HideAllInteractionUI();

  


protected:

    virtual void BeginPlay() override;

    /** Event to allow Blueprint-level logic to easily update key prompts inside the widget text */
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|UI")
    void OnInteractPromptShown(UInteractPromptWidget* PromptWidget, const FText& BoundKeyText);


private:

    /** Internally manages the logic for collecting Notes and Physical Objects */
    void HandleCollectibleCollection(AICC_Player* Player);

    /** The 3D container of the Widget. */
    UPROPERTY(VisibleAnywhere, Category = "Interaction|UI")
    TObjectPtr<UWidgetComponent> InteractionWidgetComponent;

    UPROPERTY()
    TObjectPtr<UDistantCircleWidget> DistantCircleWidgetInstance;

    UPROPERTY()
    TObjectPtr<UInteractPromptWidget> InteractPromptWidgetInstance;

    enum class ECurrentWidgetType : uint8
    {
        None,
        DistantCircle,
        InteractPrompt
    };

    ECurrentWidgetType CurrentWidget = ECurrentWidgetType::None;

   
};
