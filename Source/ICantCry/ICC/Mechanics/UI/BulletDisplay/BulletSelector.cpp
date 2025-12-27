// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletSelector.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/UI/MagazineBullet/MagazineBullet.h"

bool UBulletSelector::gCanSelect;

void UBulletSelector::Setup(FBullet& NewBullet, int32 InQuantity)
{
	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	Player = Instance->GetCurrentPlayer();
	BulletRef = NewBullet;
	BulletRefPtr = &NewBullet;
	
	const UBulletData* Data = NewBullet.GetBulletData();
	
	if (!Data) return;

	if (BulletIcon && Data->Icon)
	{
		BulletIcon->SetBrushFromTexture(Data->Icon, true);
	}
}

void UBulletSelector::DisplayBulletInfo() // NB no need to display quantity here since now it's directly bind in battle hud
{
	if (!BulletRefPtr || !BulletRefPtr->GetBulletData())
	{
		return;
	}
	
	Player->GetBattleHUD()->BulletName->SetText(FText::FromString(BulletRefPtr->GetBulletData()->BulletName));
	Player->GetBattleHUD()->Description->SetText(FText::FromString(BulletRefPtr->GetBulletData()->Description));
}

FBullet& UBulletSelector::GetBullet()
{
	return BulletRef;
}

FBullet* UBulletSelector::GetBulletPtr()
{
	return BulletRefPtr;
}

void UBulletSelector::Refresh()
{
	const TArray<URevolverSlot*>& Slots = Player->GetBattleHUD()->RevolverSlots;
	const TArray<UImage*>& PistolMagazines = Player->GetBattleHUD()->PistolMagazines;
	const TArray<UMagazineBullet*>& MagazineBullets = Player->GetBattleHUD()->MagazineBullets;
	UCircularBulletBuffer* Buffer = Player->GetBattleHUD()->GetCircularBulletBuffer();

	if (!Buffer)
	{
		return;
	}

	const int32 TailIndex = Buffer->GetTailIndex();
	const int32 Capacity = Buffer->GetCapacity();

	for (int32 i = 0; i < MagazineBullets.Num(); ++i)
	{
		const int32 BufferIndex = (TailIndex + i) % Capacity;
		UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

		UButton* BulletMagazine = MagazineBullets[i]->GetMagazineBulletButton();

		if (BulletData)
		{
			BulletMagazine->SetRenderOpacity(1.0f);
			BulletMagazine->SetBackgroundColor(BulletData->DisplayColor);
		}
		else
		{
			BulletMagazine->SetRenderOpacity(0.0f);
			BulletMagazine->SetBackgroundColor(FLinearColor::White);
		}
	}

	for (int32 i = 0; i < PistolMagazines.Num(); ++i)
	{
		int32 BufferIndex = (TailIndex + i) % Capacity;
		UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

		UImage* BulletImage = PistolMagazines[i];
		if (!BulletImage) continue;

		// Reset tint and opacity
		BulletImage->SetBrushTintColor(FSlateColor{FColor::White});
		BulletImage->SetColorAndOpacity(FLinearColor::White);

		if (BulletData)
		{
			BulletImage->SetBrushFromTexture(BulletData->Icon, true);
		}
		else
		{
			BulletImage->SetBrushFromTexture(nullptr); 
		}
	}

	Bullet->SetVisibility(ESlateVisibility::Visible);
	Bullet->SetIsEnabled(true);
}

bool UBulletSelector::CanSelect()
{
	return gCanSelect;
}

void UBulletSelector::SetCanSelect(const bool& InCanSelect)
{
	gCanSelect = InCanSelect;
}

void UBulletSelector::SetCanSelectBullet(const bool& InCanSelect)
{
	bCanSelect = InCanSelect;

	if (UButton* Button = Bullet)
	{
		Button->SetIsEnabled(bCanSelect);
	}
}

void UBulletSelector::NativeConstruct()
{
	Super::NativeConstruct();

	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Player = Cast<AICC_Player>(Controller->GetPawn());
	
	Bullet->OnClicked.AddDynamic(this, &UBulletSelector::AddToRevolver);

	Bullet->OnHovered.AddDynamic(this, &UBulletSelector::DisplayBulletInfo);

	Bullet->SetVisibility(ESlateVisibility::Visible);
	Bullet->SetIsEnabled(true);
}

void UBulletSelector::AddToRevolver()
{
	// if (!gCanSelect)
	// {
	// 	return;
	// }
	
	DebugHelper::LogSuccess("Bullet " +  BulletRefPtr->GetBulletData()->BulletName + " inserted");

	if (BulletRefPtr->GetQuantity() <= 0)
	{
		DebugHelper::LogWarning(BulletRef.GetBulletData()->BulletName + " is empty");
		return;
	}

	if (Player->GetBattleHUD()->GetCircularBulletBuffer()->IsFull())
	{
		DebugHelper::LogWarning("Revolver is already full");
		return;
	}
	
	Player->GetBattleHUD()->GetCircularBulletBuffer()->AddBullet(BulletRefPtr->GetBulletData());
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	BulletRefPtr->SetQuantity(BulletRefPtr->GetQuantity() - 1);
	DebugHelper::LogMessage(8, FColor::Red, "Decreasing quantity of " + BulletRefPtr->GetBulletData()->BulletName + ": " + FString::FromInt(BulletRefPtr->GetQuantity()));
	
	const TArray<URevolverSlot*>& Slots = Player->GetBattleHUD()->RevolverSlots;
	const TArray<UImage*>& PistolMagazines = Player->GetBattleHUD()->PistolMagazines;
	const TArray<UMagazineBullet*>& MagazineBullets = Player->GetBattleHUD()->MagazineBullets;
	
	UCircularBulletBuffer* Buffer = Player->GetBattleHUD()->GetCircularBulletBuffer();
	const int32 NumSlots = MagazineBullets.Num();
	const int32 NumBullets = Buffer->GetCount();
	const int32 Count = Buffer->GetCount();
	int32 FilledSlots = 0;

	for (int32 i = 0; i < Buffer->GetCount(); ++i)
	{
		const int32 BufferIndex = (Buffer->GetTailIndex() + i) % Buffer->GetCapacity();
		UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

		if (!BulletData)
		{
			continue;
		}

		if (FilledSlots >= NumSlots)
			break;

		UMagazineBullet* MagazineSlot = MagazineBullets[FilledSlots];
		MagazineSlot->Setup(Buffer, BulletData, BufferIndex);
		MagazineSlot->SetRenderOpacity(1.0f);
		MagazineSlot->GetMagazineBulletButton()->SetBackgroundColor(BulletData->DisplayColor);
		MagazineSlot->GetMagazineBulletButton()->SetIsEnabled(true);

		FilledSlots++;
	}

	for (int32 i = FilledSlots; i < NumSlots; ++i)
	{
		if (UMagazineBullet* MagazineSlot = MagazineBullets[i]; MagazineSlot->GetMagazineBulletButton()->GetIsEnabled())
		{
			MagazineSlot->Setup(Buffer, nullptr, -1);
			MagazineSlot->SetRenderOpacity(0.25f);
			MagazineSlot->GetMagazineBulletButton()->SetBackgroundColor(FLinearColor::Transparent);
			MagazineSlot->GetMagazineBulletButton()->SetIsEnabled(false);
		}
	}

	for (int32 i = 0; i < PistolMagazines.Num(); ++i)
	{
		const int32 BufferIndex = (Player->GetBattleHUD()->GetCircularBulletBuffer()->GetTailIndex() + i) % Player->GetBattleHUD()->GetCircularBulletBuffer()->GetCapacity();
		const UBulletData* BulletData = Player->GetBattleHUD()->GetCircularBulletBuffer()->PeekAt(BufferIndex);

		if (const UImage* BulletImage = PistolMagazines[i]; BulletImage)
		{
			if (BulletData)
			{
				PistolMagazines[i]->SetBrushTintColor(FSlateColor{FColor::White});
				PistolMagazines[i]->SetColorAndOpacity(FLinearColor::White);
				PistolMagazines[i]->SetBrushFromTexture(BulletData->Icon, true);
			}
			else
			{
				PistolMagazines[i]->SetBrushFromTexture(nullptr);
			}
		}
	}
	
	Player->GetBattleHUD()->RefreshBulletMagazine();
}


