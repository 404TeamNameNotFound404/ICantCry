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

	if (!DebugHelper::IsGamepadPlugged())
	{
		Player->GetBattleHUD()->SetHoveredSelectedBullet(this);
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

bool UBulletSelector::CanBeSelected() const
{
	return bCanSelect;
}

void UBulletSelector::SetCanBeSelected(const bool& Value)
{
	bCanSelect = Value;
}

void UBulletSelector::SetCanSelectBullet(const bool& InCanSelect)
{
	bCanSelect = InCanSelect;

	if (UButton* Button = Bullet)
	{
		Button->SetIsEnabled(bCanSelect);
	}
}

UButton* UBulletSelector::GetButton()
{
	return Bullet;
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
	if (DebugHelper::IsGamepadPlugged())
	{
		if (!bCanSelect)
		{
			DebugHelper::LogError("b is not selected");
			return;
		}
	
		if (const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletSelector() != this)
		{
			DebugHelper::LogError("b is not this");
			return;
		}
		
		if (AICC_PlayerController* PC = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController()); PC)
		{
			const FInputModeGameAndUI InputMode;
			PC->SetInputMode(InputMode);

			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
			PC->bShowMouseCursor = false;
			PC->GetLocalPlayer()->ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
		}
		
	}

	UCircularBulletBuffer* Buffer = Player->GetBattleHUD()->GetCircularBulletBuffer();

	if (Buffer->IsEmpty())
	{
		Buffer->Clear();
	}
	
	if (const bool bIsEmpty = BulletRefPtr->GetQuantity() <= 0; bIsEmpty)
	{
		DebugHelper::LogWarning(BulletRefPtr->GetBulletData()->BulletName + " is empty");
		return;
	}
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	if ((DebugHelper::IsGamepadPlugged() && Player->GetBattleHUD()->GetCircularBulletBuffer()->IsFull()) ||
		(DebugHelper::IsGamepadPlugged() && Instance->GetInventory().BulletsStored.IsEmpty()))
	{
		Player->GetBinder()->SetIsNavigatingInsideWidget(false); 
		Player->GetBinder()->FocusOn(Player->GetBattleHUD()->GetBulletDisplayer()->GetBulletConfirmGamepad());
	}
	
	if (Player->GetBattleHUD()->GetCircularBulletBuffer()->IsFull())
	{
		DebugHelper::LogWarning("Revolver is already full");
		return;
	}
	
	
	
	Buffer->AddBullet(BulletRefPtr->GetBulletData());
	

	BulletRefPtr->SetQuantity(BulletRefPtr->GetQuantity() - 1);
	
	const TArray<UImage*>& PistolMagazines = Player->GetBattleHUD()->PistolMagazines;
	const TArray<UMagazineBullet*>& MagazineBullets = Player->GetBattleHUD()->MagazineBullets;
	
	const int32 NumSlots = MagazineBullets.Num();

	for (int32 i = 0; i < NumSlots; ++i)
	{
		UMagazineBullet* MagazineSlot = MagazineBullets[i];

		const int32 BufferIndex = (Buffer->GetTailIndex() + i) % Buffer->GetCapacity();
		UBulletData* BulletData = Buffer->PeekAt(BufferIndex);

		MagazineSlot->SetEnableRemoval(BulletData != nullptr);
		MagazineSlot->Setup(Buffer, BulletData, BufferIndex);
	}

	Player->GetBattleHUD()->RefreshPistolMagazine();
}


