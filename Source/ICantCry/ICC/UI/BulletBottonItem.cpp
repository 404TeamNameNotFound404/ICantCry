// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBottonItem.h"
#include "InventoryHUD.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"


void UBulletBottonItem::NativeConstruct()
{
	Super::NativeConstruct();

	SelectButton->OnClicked.AddDynamic(this, &UBulletBottonItem::DisplayBulletInfo);
	//SelectButton->OnUnhovered.AddDynamic(this, &UBulletBottonItem::HideBulletInfo);

	if (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance()))
	{
		Instance->GetInventory().OnBulletCrafted.AddUObject(this, &UBulletBottonItem::UpdateQuantity);
	}
}

void UBulletBottonItem::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	DisplayBulletInfo();
	
	if (const UWidget* Parent = GetParent())
	{
		if (UScrollBox* ScrollBox = Cast<UScrollBox>(Parent->GetParent()))
		{
			ScrollBox->ScrollWidgetIntoView(this, true, EDescendantScrollDestination::IntoView, 0.0f);
		}
	}
}

void UBulletBottonItem::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	HideBulletInfo();
}


void UBulletBottonItem::Setup(const FBullet& NewBullet, int32 InQuantity)
{
	// MyBullet = NewBullet;

	// UBulletData* Data = MyBullet.GetBulletData();
	// if (!Data) return;

	// if (BulletIconImage && Data->Icon)
	// {
	//     BulletIconImage->SetBrushFromTexture(Data->Icon, true);
	// }

	// if (BulletNameText)
	// {
	//     BulletNameText->SetText(FText::FromString(Data->BulletName));
	// }

	// if (BulletQuantityText)
	// {
	//     BulletQuantityText->SetText(FText::Format(NSLOCTEXT("Inventory", "QuantityFormat", "x{0}"), FText::AsNumber(InQuantity)));
	// }


	MyBullet = NewBullet;
	UBulletData* Data = MyBullet.GetBulletData();
	if (!Data) return;

	if (BulletIconImage && Data->Icon)
	{
		BulletIconImage->SetBrushFromTexture(Data->Icon, true);
		BulletIconImage->SetVisibility(ESlateVisibility::Visible); // AGGIUNTO
	}

	if (BulletNameText)
	{
		BulletNameText->SetText(FText::FromString(Data->BulletName));
		BulletNameText->SetVisibility(ESlateVisibility::Visible); // AGGIUNTO
	}

	if (BulletQuantityText)
	{
		BulletQuantityText->SetText(
			FText::Format(NSLOCTEXT("Inventory", "QuantityFormat", "x{0}"), FText::AsNumber(InQuantity)));
		BulletQuantityText->SetVisibility(ESlateVisibility::Visible); // AGGIUNTO
	}

	// Anche il bottone deve essere abilitato
	if (SelectButton)
	{
		SelectButton->SetIsEnabled(true);
	}
}

void UBulletBottonItem::SetSelected(bool bIsSelected)
{
	if (SelectionBorder)
	{
		SelectionBorder->SetBrushColor(bIsSelected ? FLinearColor::Green : FLinearColor::Transparent);
	}
}

void UBulletBottonItem::SetOwner(UInventoryHUD* Owner, int32 Index)
{
	OwnerHUD = Owner;
	MyIndex = Index;

	if (SelectButton)
	{
		SelectButton->OnHovered.AddDynamic(this, &UBulletBottonItem::OnButtonClicked);
	}
}

void UBulletBottonItem::SetOwner(UInventoryHUD* Owner)
{
	OwnerHUD = Owner;
}

bool UBulletBottonItem::IsHoverSelected() const
{
	return bIsHovered;
}

void UBulletBottonItem::SetIsSelected(const bool& Value)
{
	bIsHovered = Value;
}

UImage* UBulletBottonItem::GetBulletIcon()
{
	return BulletIconImage;
}

UTextBlock* UBulletBottonItem::GetBulletName()
{
	return BulletNameText;
}

UTextBlock* UBulletBottonItem::GetBulletQuantity()
{
	return BulletQuantityText;
}

UButton* UBulletBottonItem::GetBulletButton()
{
	return SelectButton;
}

bool UBulletBottonItem::GetIsUnlocked() const
{
	return bIsUnlocked;
}

void UBulletBottonItem::SetIsUnlocked(const bool& Value)
{
	bIsUnlocked = Value;
}

// questo è entry point di tutto e verrà chiamato anche per le blueprint ottenute invece di aggiungerle perche sono gia aggiunte tutte
// noi le nascondiamo di default ma gia le blueprint le settiamo per ogni bottone creato 
void UBulletBottonItem::SetUnlocked(bool bUnlocked) 
{
	bIsUnlocked = bUnlocked;

	if (SelectButton)
	{
		SelectButton->SetIsEnabled(bUnlocked);

		SelectButton->SetVisibility(
			bUnlocked ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible
		);
	}
}

void UBulletBottonItem::Show()
{
	// SelectButton->SetIsEnabled(true);
	// BulletIconImage->SetVisibility(ESlateVisibility::Visible);
	// BulletNameText->SetVisibility(ESlateVisibility::Visible);
	// BulletQuantityText->SetVisibility(ESlateVisibility::Visible);

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	const bool bUnlocked = GetIsUnlocked();

	SelectButton->SetIsEnabled(bUnlocked);

	const ESlateVisibility Vis =
		bUnlocked ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	BulletIconImage->SetVisibility(Vis);
	BulletNameText->SetVisibility(Vis);
	BulletQuantityText->SetVisibility(Vis);

	if (!Instance->GetInventory().BulletsStored.IsEmpty())
	{
		if (!MyBullet.IsValid() || !MyBullet.GetBulletData())
		{
			return;
		}
		
		BulletQuantityText->SetText(FText::FromString("x " + FString::FromInt(Instance->GetInventory().BulletsStored[MyBullet.GetBulletData()->Type].GetQuantity())));
	}
	else
	{
		if (!MyBullet.IsValid() || !MyBullet.GetBulletData())
		{
			return;
		}
		
		BulletQuantityText->SetText(FText::FromString("x 0"));
	}
}

void UBulletBottonItem::Hide()
{
	SelectButton->SetIsEnabled(false);
	BulletIconImage->SetVisibility(ESlateVisibility::Hidden);
	BulletNameText->SetVisibility(ESlateVisibility::Hidden);
	BulletQuantityText->SetVisibility(ESlateVisibility::Hidden);
}

FRecipe& UBulletBottonItem::GetBlueprint()
{
	return BulletBlueprint;
}

void UBulletBottonItem::OnButtonClicked()
{
	if (OwnerHUD)
	{
		OwnerHUD->SelectBullet(MyIndex);
	}
}

FText UBulletBottonItem::RefreshQuantity()
{
	if (!MyBullet.GetBulletData())
	{
		return FText::FromString("x 0");
	}
	
	for (UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		auto B : Instance->GetInventory().BulletsStored)
	{
		FBullet& Bullet = B.Value;
		
		if (!Bullet.GetBulletData())
		{
			continue;
		}
		
		if (Bullet.GetBulletData()->Type == MyBullet.GetBulletData()->Type)
		{
			const FString Txt = FString("x " + FString::FromInt(Bullet.GetQuantity()));
			return FText::FromString(Txt);
		}
	}
	
	return FText::FromString("x 0");
}

void UBulletBottonItem::UpdateQuantity()
{
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
	{
		BulletQuantityText->SetText(RefreshQuantity());
	}, 0.24f, false);
}

void UBulletBottonItem::DisplayBulletInfo()
{
	const AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	if (const AICC_Player* Player = Cast<AICC_Player>(Controller->GetPawn()); Player->bIsInFight)
	{
		return;
	}

	bIsHovered = true;

	if (!OwnerHUD)
	{
		return;
	}

	OwnerHUD->BlueprintRequirementTxt->SetText(FText::FromString(FString(BulletBlueprint.GetCaseType(BulletBlueprint.RequiredCasingType) + ": " +
		FString::FromInt(BulletBlueprint.Requirements->CasingQuantity) + " - " +  BulletBlueprint.GetEssencesName() + " " +  FString::FromInt(BulletBlueprint.Requirements->EssenceQuantity))));

	OwnerHUD->SelectedBulletImage->SetBrushFromTexture(MyBullet.GetBulletData()->Icon);
	OwnerHUD->SelectedBulletName->SetText(FText::FromString(MyBullet.GetBulletData()->BulletName));
	OwnerHUD->CraftInfo->SetText(FText::FromString(DisplayIngredients()));
	OwnerHUD->SelectedBulletPower->SetText(
		FText::FromString("Bullet Power: " + FString::FromInt(MyBullet.GetBulletData()->Power)));
	OwnerHUD->SelectedBulletEffectiveness->SetText(
		FText::FromString("Strong against: " + MyBullet.GetStrongAgainstName()));
	OwnerHUD->SelectedBulletWeakness->SetText(FText::FromString("Weak against: " + MyBullet.GetWeakAgainstName()));

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	Instance->GetInventory().SetSelectedRecipe(BulletBlueprint);

	OwnerHUD->GetTable()->ScanResources(GetWorld());
	OwnerHUD->CraftButton->SetIsEnabled(false);
	
	if (OwnerHUD->GetTable()->CanCraft())
	{
		DebugHelper::LogSuccess(OwnerHUD->SelectedBulletName->GetText().ToString() + " can be crafted");
		OwnerHUD->CraftButton->SetIsEnabled(true);
	}

}

FString UBulletBottonItem::DisplayIngredients()
{
	FString EssenceName = "";
	int32 Quantity = 0;
	
	if (BulletBlueprint.GetRequiredEssences().IsEmpty()) return FString("Crafted With - Empty");
	
	for (FEssence& Essence : BulletBlueprint.GetRequiredEssences())
	{
		if (!Essence.IsValid()) continue;
		EssenceName = Essence.GetName(Essence.EssenceType);
		Quantity = Essence.Quantity;
	}
	
	const int32& CasingQuantity = BulletBlueprint.RequiredCasingQuantity;
	// Note for myself if essences are multiple i think it can be better if i move the below line inside the loop otherwise the EssenceName and Quantity will only be valid for the last one
	return FString("Crafted With: " + FString::FromInt(Quantity) + " " + EssenceName + " " + FString::FromInt(CasingQuantity) + " " + "Indifference");
}

void UBulletBottonItem::HideBulletInfo()
{
	bIsHovered = false;
}
