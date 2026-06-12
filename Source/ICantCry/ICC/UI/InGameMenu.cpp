// Fill out your copyright notice in the Description page of Project Settings.
#include "InGameMenu.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Inventory/CraftingTable.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/ICC_EnhancedInputCmp.h"
#include "ICantCry/ICC/Input/Tags/ICC_InputTags.h"

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	Character->OnClicked.AddDynamic(this, &UInGameMenu::OpenCharacter);
	Inventory->OnClicked.AddDynamic(this, &UInGameMenu::OpenInventory);
	// Map->OnClicked.AddDynamic(this, &UInGameMenu::OpenMap);
	

	// For debugging purposes only!
	Map->SetIsEnabled(false);


	if (!InventoryHud)
	{
		InventoryHud = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDClass);
		InventoryHud->SetVisibility(ESlateVisibility::Hidden);
		InventoryHud->Setup();
	}

	
	if (!CharacterUI)
	{
		CharacterUI = CreateWidget<UCharacterUI>(GetWorld(), CharacterUIClass);
		CharacterUI->SetVisibility(ESlateVisibility::Visible);
		Main->AddChild(CharacterUI);
		CharacterUI->RefreshUI();
	}
	
	Hightlight(Character);
	
	Scrollers = {Character, Inventory, Map};
	
	Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	FTimerHandle BindDelay;
	GetWorld()->GetTimerManager().SetTimer(BindDelay, this, &UInGameMenu::Bind, 0.4f, false);
	
}

void UInGameMenu::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	
}

void UInGameMenu::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void UInGameMenu::LeftNavigation()
{
	NavigateThroughtButtons(-1);
	DebugHelper::LogMessage(5,FColor::White, "Im supposed to scroll left");
}

void UInGameMenu::RightNavigation()
{
	NavigateThroughtButtons(1);
	DebugHelper::LogMessage(5,FColor::White, "Im supposed to scroll right");
}

void UInGameMenu::NavigateThroughtButtons(const int32& InDirection)
{
	if (Scrollers.IsEmpty()) return;

	const int32 OriginalIndex = ScrollerIndex;
	bool bFoundValidButton = false;
	
	for (int32 i = 0; i < Scrollers.Num(); ++i)
	{
		ScrollerIndex = (ScrollerIndex + InDirection + Scrollers.Num()) % Scrollers.Num();

		if (const UButton* Target = Scrollers[ScrollerIndex])
		{
			if (Target->GetIsEnabled() && Target->GetVisibility() == ESlateVisibility::Visible)
			{
				bFoundValidButton = true;
				break; 
			}
		}
	}
	
	if (bFoundValidButton)
	{
		if (UButton* Target = Scrollers[ScrollerIndex])
		{
			Target->SetFocus();
			Hightlight(Target);
		}
	}
	else
	{
		ScrollerIndex = OriginalIndex;
	}
}

void UInGameMenu::Bind()
{
	UICC_EnhancedInputCmp* Binder = Cast<UICantCryGameInstance>(GetGameInstance())->GetCurrentPlayer()->GetInputBinder();
	UICC_InputDataAsset* Data = Cast<UICantCryGameInstance>(GetGameInstance())->GetCurrentPlayer()->GetInputDataAsset();
	Binder->BindNativeInputAction(Data ,Icc_InputTags::InputTag_ScrollMainMenuLeft, ETriggerEvent::Triggered,this , &UInGameMenu::LeftNavigation);
	Binder->BindNativeInputAction(Data ,Icc_InputTags::InputTag_ScrollMainMenuRight, ETriggerEvent::Triggered,this , &UInGameMenu::RightNavigation);
	Binder->BindNativeInputAction(Data, Icc_InputTags::InputTag_Interact, ETriggerEvent::Triggered, this, &UInGameMenu::PadClick);
}

void UInGameMenu::PadClick()
{
	switch (ScrollerIndex)
	{
	case 0: 
		OpenCharacter(); // Call the same function your Mouse uses!
		break;

	case 1:
		OpenInventory();
		break;

	case 2: 
		// OpenMap(); 
		break;

	default:
		break;
	}
}


void UInGameMenu::Hightlight(UWidget* What)
{
	if (!What || !OverviewFrame) return;
	
	OverviewFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UCanvasPanelSlot* FrameSlot = Cast<UCanvasPanelSlot>(OverviewFrame->Slot);
	
	if (const UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(What->Slot);
		FrameSlot && ButtonSlot)
	{
		FrameSlot->SetPosition(ButtonSlot->GetPosition());
		FrameSlot->SetSize(ButtonSlot->GetSize());
		
		FrameSlot->SetAnchors(ButtonSlot->GetAnchors());
		FrameSlot->SetAlignment(ButtonSlot->GetAlignment());
	}
}


/*
 * Il ragionamento di base è -> Io Apro il menu ---[Character]--------[Inventory]-------------[Map]---
 *                                                      |
 *                                                      |
 *                                           Default View---AddChild CharaUI <- Remove Child ->AddChild Inv
 *                                                      |         |
 *                                                      | -----< ---- - Click->     |
 *
 *    Disegno un po del cazzo ma in sostanza carichiamo tutto in native constructor poi premi C ,
 *    si apre il menu con character ui come window principale
 *    clicchi inventory, characterui viene rimossa da child e nascosta per aggiungere inventory e viceversa
 *    il crash del cazzo avveniva perche refresh veniva chiamato sul "else" quando ancora non stava in gioco la ui, refresh si fa una volta sola ogni volta che
 *    viene cliccato.
 */

void UInGameMenu::OpenInventory()
{
	if (CharacterUI)
	{
		Main->RemoveChild(CharacterUI);
	}
	
	Main->AddChild(InventoryHud);
	InventoryHud->Refresh();
	InventoryHud->SetVisibility(ESlateVisibility::Visible);
	Hightlight(Inventory);
	InventoryHud->SetFocus();
	InventoryHud->SetUserFocus(Controller);
}



void UInGameMenu::OpenCharacter()
{

	if (InventoryHud)
	{
		Main->RemoveChild(InventoryHud);
		InventoryHud->SetVisibility(ESlateVisibility::Hidden);
	}
	
	Main->AddChild(CharacterUI);
	CharacterUI->RefreshUI();
	//InventoryHud->SetVisibility(ESlateVisibility::Hidden);
	CharacterUI->SetVisibility(ESlateVisibility::Visible);

	Hightlight(Character);
	Character->SetUserFocus(Controller);
}



void UInGameMenu::OpenMap()
{
	
}

void UInGameMenu::InstantiateTable(AICC_Player* Player)
{
	Table = NewObject<UCraftingTable>(Player);
	checkf(Table, TEXT("Table not initialized"))
}

void UInGameMenu::SetMenuOpen(const bool& Value)
{
	bMenuOpen = Value;
}

bool UInGameMenu::IsOpen() const
{
	return bMenuOpen;
}

void UInGameMenu::SetDisabled(const bool& Value)
{
	bDisable = Value;
}

bool UInGameMenu::IsDisabled() const
{
	return bDisable;
}

UCharacterUI* UInGameMenu::GetCharacterUI()
{
	return CharacterUI;
}
