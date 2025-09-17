// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletSelector.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


void UBulletSelector::Setup(FBullet& NewBullet, int32 InQuantity)
{
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
	UCircularBulletBuffer* Buffer = Player->GetBattleHUD()->GetCircularBulletBuffer();

	if (!Buffer)
	{
		return;
	}

	const int32 TailIndex = Buffer->GetTailIndex();
	const int32 Capacity = Buffer->GetCapacity();

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		int32 BufferIndex = (TailIndex + i) % Capacity;
		UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

		UImage* BulletImage = Slots[i]->BulletIcon;
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
}

void UBulletSelector::NativeConstruct()
{
	Super::NativeConstruct();

	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	Player = Cast<AICC_Player>(Controller->GetPawn());
	
	Bullet->OnClicked.AddDynamic(this, &UBulletSelector::AddToRevolver);

	Bullet->OnHovered.AddDynamic(this, &UBulletSelector::DisplayBulletInfo);
}

void UBulletSelector::AddToRevolver()
{
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
	
	//Player->GetBattleHUD()->GetCircularBulletBuffer()->AddBullet(BulletRef.GetBulletData());
	Player->GetBattleHUD()->GetCircularBulletBuffer()->AddBullet(BulletRefPtr->GetBulletData());
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	BulletRefPtr->SetQuantity(BulletRefPtr->GetQuantity() - 1);
	DebugHelper::LogMessage(8, FColor::Red, "Decreasing quantity of " + BulletRefPtr->GetBulletData()->BulletName + ": " + FString::FromInt(BulletRefPtr->GetQuantity()));
	
	const TArray<URevolverSlot*>& Slots = Player->GetBattleHUD()->RevolverSlots;
	const TArray<UImage*>& PistolMagazines = Player->GetBattleHUD()->PistolMagazines;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		int32 BufferIndex = (Player->GetBattleHUD()->GetCircularBulletBuffer()->GetTailIndex() + i) % Player->GetBattleHUD()->GetCircularBulletBuffer()->GetCapacity();
		UBulletData* BulletData = Player->GetBattleHUD()->GetCircularBulletBuffer()->PeekAt(BufferIndex);

		UImage* BulletImage = Slots[i]->EmptySlotIcon;
		BulletIcon->SetBrushTintColor(FSlateColor{FColor::White});
		BulletIcon->SetColorAndOpacity(FLinearColor::White);
		Slots[i]->BulletIcon->SetBrushTintColor(FSlateColor{FColor::White});
		Slots[i]->BulletIcon->SetColorAndOpacity(FLinearColor::White);
		

		if (BulletImage)
		{
			if (BulletData)
			{
				BulletImage->SetBrushFromTexture(BulletData->Icon, true);
			}
			else
			{
				BulletImage->SetBrushFromTexture(nullptr); // Clear unused slot
			}
		}
	}


	for (int32 i = 0; i < PistolMagazines.Num(); ++i)
	{
		int32 BufferIndex = (Player->GetBattleHUD()->GetCircularBulletBuffer()->GetTailIndex() + i) % Player->GetBattleHUD()->GetCircularBulletBuffer()->GetCapacity();
		UBulletData* BulletData = Player->GetBattleHUD()->GetCircularBulletBuffer()->PeekAt(BufferIndex);

		UImage* BulletImage = PistolMagazines[i];

		if (BulletImage)
		{
			if (BulletData)
			{
				PistolMagazines[i]->SetBrushTintColor(FSlateColor{FColor::White});
				PistolMagazines[i]->SetColorAndOpacity(FLinearColor::White);
				PistolMagazines[i]->SetBrushFromTexture(BulletData->Icon, true);
			}
			else
			{
				PistolMagazines[i]->SetBrushFromTexture(nullptr); // Clear unused slot
			}
		}
	}
}


