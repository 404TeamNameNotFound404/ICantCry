// Fill out your copyright notice in the Description page of Project Settings.
#include "MagazineBullet.h"

#include "Components/Image.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UMagazineBullet::RemoveFromMagazine()
{
	if (!Buffer)
		return;
	
	Buffer->RemoveAt(BulletIndex);
	DebugHelper::LogSuccess("Removed bullet from slot index " + FString::FromInt(BulletIndex));
	FBullet* Bullet = Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentSelectedBullet();
	Bullet->SetQuantity(Bullet->GetQuantity() + 1);
	DebugHelper::LogMessage(5, FColor::White, "Re adding " + Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentSelectedBullet()->GetBulletData()->BulletName);
	BulletBtn->SetBackgroundColor(FLinearColor::Transparent);
	Setup(nullptr, nullptr, -1);
}

UButton* UMagazineBullet::GetMagazineBulletButton() const
{
	return BulletBtn;
}

void UMagazineBullet::SetBufferIndex(const int32& NewIndex)
{
	BulletIndex = NewIndex;
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
			BulletBtn->SetBackgroundColor(BulletDataPtr->DisplayColor);
			BulletBtn->SetIsEnabled(true);
			SetRenderOpacity(1.0f);
		}
		else
		{
			BulletBtn->SetBackgroundColor(FLinearColor::Transparent);
			BulletBtn->SetIsEnabled(false);
			SetRenderOpacity(0.25f);
		}
	}

	// if (BulletBtn && BulletData)
	// {
	// 	BulletBtn->SetBackgroundColor(BulletData->DisplayColor);
	// }

	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
}


void UMagazineBullet::NativeConstruct()
{
	Super::NativeConstruct();
	DebugHelper::LogMessage(7,FColor::White,"Native Construct from umagazinebulelt called");
	BulletBtn->OnClicked.AddDynamic(this, &UMagazineBullet::RemoveFromMagazine);
	DebugHelper::LogSuccess("Bound OnClicked for bullet index " + FString::FromInt(BulletIndex));
}
