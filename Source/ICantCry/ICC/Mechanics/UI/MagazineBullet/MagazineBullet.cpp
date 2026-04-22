// Fill out your copyright notice in the Description page of Project Settings.
#include "MagazineBullet.h"

#include "Components/Image.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UMagazineBullet::RemoveFromMagazine()
{
	if (!bEnableRemoval || !Buffer)
		return;
	
	if (BulletIndex < 0 || BulletIndex >= Buffer->GetCapacity())
	{
		DebugHelper::LogWarning("Invalid BulletIndex: " + FString::Printf(TEXT("%d"), BulletIndex));
		return;
	}
	
	Buffer->RemoveAt(BulletIndex);

	if (FBullet* Bullet = Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentSelectedBullet(); Bullet)
	{
		const int32 NewQuantity = Bullet->GetQuantity() + 1;
		Bullet->SetQuantity(NewQuantity);
	}
	else
	{
		DebugHelper::LogWarning("No selected bullet found to update.");
	}
	
	BulletBtn->SetBackgroundColor(FLinearColor::Transparent);
	Setup(nullptr, nullptr, -1);
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->RefreshPistolMagazine();
	
	// Buffer->RemoveAt(BulletIndex);
	// FBullet* Bullet = Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentSelectedBullet();
	// Bullet->SetQuantity(Bullet->GetQuantity() + 1);
	// BulletBtn->SetBackgroundColor(FLinearColor::Transparent);
	// Setup(nullptr, nullptr, -1);
	//
	// Instance->GetCurrentPlayer()->GetBattleHUD()->RefreshPistolMagazine();
}

UButton* UMagazineBullet::GetMagazineBulletButton() const
{
	return BulletBtn;
}

void UMagazineBullet::SetBufferIndex(const int32& NewIndex)
{
	BulletIndex = NewIndex;
}

int32 UMagazineBullet::GetBulletIndex() const
{
	return BulletIndex;
}

bool UMagazineBullet::IsAllowedToRemoveBullet() const
{
	return bEnableRemoval;
}

void UMagazineBullet::SetEnableRemoval(const bool& NewEnableRemoval)
{
	bEnableRemoval = NewEnableRemoval;
}

void UMagazineBullet::Clear()
{
	if (!bEnableRemoval || !Buffer)
		return;

	if (Buffer->IsEmpty())
	{
		return;
	}
	
	Instance->GetInventory().BulletsStored = Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetEnemySpawnManager()->GetMemory().InBattleBullets;
	
	Buffer->Clear();
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->Refresh();
	Instance->GetCurrentPlayer()->GetBattleHUD()->RefreshPistolMagazine();
	Instance->GetCurrentPlayer()->GetBattleHUD()->RefreshBulletMagazine();
}


void UMagazineBullet::Setup(UCircularBulletBuffer* InBuffer,  UBulletData* BulletData, int32 Index)
{
	Buffer = InBuffer;
	BulletIndex = Index;
	
	BulletDataPtr = BulletData;
	
	if (BulletBtn)
	{
		if (BulletDataPtr)
		{
			bEnableRemoval = true;
			BulletBtn->SetVisibility(ESlateVisibility::Visible);
			
			BulletBtn->SetBackgroundColor(BulletDataPtr->DisplayColor);
			BulletBtn->SetColorAndOpacity(FLinearColor::Transparent);
			SetRenderOpacity(1.0f);
		}
		else
		{
			BulletBtn->SetBackgroundColor(FLinearColor::Transparent);
			BulletBtn->SetColorAndOpacity(FLinearColor::Transparent);
			bEnableRemoval = false;
			BulletBtn->SetVisibility(ESlateVisibility::Visible);
			SetRenderOpacity(0.25f);
		}
	}

	BulletBtn->SynchronizeProperties();
	BulletBtn->InvalidateLayoutAndVolatility();
	BulletBtn->ForceLayoutPrepass();
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
}


void UMagazineBullet::NativeConstruct()
{
	Super::NativeConstruct();
	BulletBtn->OnClicked.AddDynamic(this, &UMagazineBullet::RemoveFromMagazine);
}
