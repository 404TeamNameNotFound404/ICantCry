// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBottonItem.h"
#include "InventoryHUD.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"


void UBulletBottonItem::NativeConstruct()
{
	Super::NativeConstruct();

	SelectButton->OnHovered.AddDynamic(this, &UBulletBottonItem::DisplayBulletInfo);
	SelectButton->OnUnhovered.AddDynamic(this, &UBulletBottonItem::HideBulletInfo);
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
		BulletQuantityText->SetText(FText::FromString("x " + FString::FromInt(Instance->GetInventory().BulletsStored[MyBullet.GetBulletData()->Type].GetQuantity())));
	}
	else
	{
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
	OwnerHUD->CraftInfo->SetText(FText::FromString("Crafted with -"));
	OwnerHUD->SelectedBulletPower->SetText(
		FText::FromString("Bullet Power: " + FString::FromInt(MyBullet.GetBulletData()->Power)));
	OwnerHUD->SelectedBulletEffectiveness->SetText(
		FText::FromString("Strong against: " + MyBullet.GetStrongAgainstName()));
	OwnerHUD->SelectedBulletWeakness->SetText(FText::FromString("Weak against: " + MyBullet.GetWeakAgainstName()));
}

void UBulletBottonItem::HideBulletInfo()
{
	bIsHovered = false;
}
