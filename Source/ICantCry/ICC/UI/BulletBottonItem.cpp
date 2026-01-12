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
    MyBullet = NewBullet;

    UBulletData* Data = MyBullet.GetBulletData();
    if (!Data) return;

    if (BulletIconImage && Data->Icon)
    {
        BulletIconImage->SetBrushFromTexture(Data->Icon, true);
    }

    if (BulletNameText)
    {
        BulletNameText->SetText(FText::FromString(Data->BulletName));
    }

    if (BulletQuantityText)
    {
        BulletQuantityText->SetText(FText::Format(NSLOCTEXT("Inventory", "QuantityFormat", "x{0}"), FText::AsNumber(InQuantity)));
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

bool UBulletBottonItem::IsHoverSelected() const
{
    return bIsHovered;
}

void UBulletBottonItem::SetIsSelected(const bool& Value)
{
    bIsHovered = Value;
}

UImage *UBulletBottonItem::GetBulletIcon()
{
    return BulletIconImage;
}

UTextBlock *UBulletBottonItem::GetBulletName()
{
    return BulletNameText;
}

UTextBlock *UBulletBottonItem::GetBulletQuantity()
{
    return BulletQuantityText;
}

UButton *UBulletBottonItem::GetBulletButton()
{
    return SelectButton;
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
    AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
    AICC_Player* Player = Cast<AICC_Player>(Controller->GetPawn());

    if (Player->bIsInFight)
    {
        return;
    }
    
    bIsHovered = true;

    if (!OwnerHUD)
    {
        DebugHelper::LogError("OwnerHUD is invalid");
        return;
    }

    OwnerHUD->SelectedBulletImage->SetBrushFromTexture(MyBullet.GetBulletData()->Icon);
    OwnerHUD->SelectedBulletName->SetText(FText::FromString(MyBullet.GetBulletData()->BulletName));
    OwnerHUD->CraftInfo->SetText(FText::FromString("Crafted with -" ));
    OwnerHUD->SelectedBulletPower->SetText(FText::FromString("Bullet Power: " + FString::FromInt(MyBullet.GetBulletData()->Power)));
    OwnerHUD->SelectedBulletEffectiveness->SetText(FText::FromString("Strong against: " + MyBullet.GetStrongAgainstName()));
    OwnerHUD->SelectedBulletWeakness->SetText(FText::FromString("Weak against: " + MyBullet.GetWeakAgainstName()));
}

void UBulletBottonItem::HideBulletInfo()
{
    bIsHovered = false;
}
