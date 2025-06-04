// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBottonItem.h"
#include "InventoryHUD.h"


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
        SelectButton->OnClicked.AddDynamic(this, &UBulletBottonItem::OnButtonClicked);
    }
}

void UBulletBottonItem::OnButtonClicked()
{
    if (OwnerHUD)
    {
        OwnerHUD->SelectBullet(MyIndex);
    }
}