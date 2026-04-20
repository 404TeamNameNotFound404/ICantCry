// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletDisplayer.h"

#include "BulletSelector.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UBulletDisplayer::NativeConstruct()
{
	Super::NativeConstruct();

	Setup();
}

void UBulletDisplayer::Setup()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	BulletGrid->ClearChildren();
	Bullets.Empty();

	if (Instance->GetInventory().BulletsStored.IsEmpty())
	{
		DebugHelper::LogMessage(10, FColor::Yellow, "Inventory empty during Setup");
		return;
	}

	int32 Index = 0;
	const int32 MaxCols = 4;

	for (auto& Bullet : Instance->GetInventory().BulletsStored)
	{
		FBullet& B = Bullet.Value;
		if (B.GetQuantity() <= 0) continue;

		UBulletSelector* Item = CreateWidget<UBulletSelector>(GetWorld(), BulletButtonItemClass);
		Item->Setup(B, B.GetQuantity());
		
		const int32 Row = Index / MaxCols;
		const int32 Column = Index % MaxCols;
		
		//Item->SetPadding(FMargin(2, 2));

		UGridSlot* BulletSlot = Cast<UGridSlot>(BulletGrid->AddChildToGrid(Item, Row, Column));
		
		if (!BulletSlot)
		{
			continue;
		}
		
		BulletSlot->SetRow(Row);
		BulletSlot->SetColumn(Column);
		BulletSlot->SetPadding(FMargin(5.f));
		BulletSlot->SetHorizontalAlignment(HAlign_Fill);
		BulletSlot->SetVerticalAlignment(VAlign_Fill);

		//BulletGrid->AddChild(Item);
		Bullets.Add(Item);
		Index++;
	}

	ConfirmGamepadBtn->SetVisibility(DebugHelper::IsGamepadPlugged() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	ConfirmGamepadBtn->OnClicked.AddDynamic(Instance->GetCurrentPlayer()->GetBattleHUD(), &UBattleHUD::ConfirmBulletSelection);
}


void UBulletDisplayer::Refresh()
{
	Bullets.Empty();
	BulletGrid->ClearChildren();

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	int32 Index = 0;
	
	for (auto& Bullet : Instance->GetInventory().BulletsStored)
	{
		FBullet& B = Bullet.Value;
		if (B.GetQuantity() <= 0) continue;
	
		UBulletSelector* Item = CreateWidget<UBulletSelector>(GetWorld(), BulletButtonItemClass);
		Item->SetIsFocusable(true);
		Item->Setup(B, B.GetQuantity());
		Item->SetPadding(FMargin(2, 2));
	
		UGridSlot* BulletSlot = Cast<UGridSlot>(BulletGrid->AddChild(Item));
		if (!BulletSlot)
		{
			continue;
		}
	
		const int32 Row = Index / BulletSlotPadding;
		const int32 Column = Index % BulletSlotPadding;
	
		BulletSlot->SetRow(Row);
		BulletSlot->SetColumn(Column);
	
		//BulletGrid->AddChild(Item);
		Bullets.Add(Item);
		Index++;
	}
	
	if (GetBullets().IsValidIndex(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedBulletIndex()))
	{
		Instance->GetCurrentPlayer()->GetBattleHUD()->SetSelectedBulletIndex(0);
	}

}

TArray<UBulletSelector*> UBulletDisplayer::GetBullets() const
{
	return Bullets;
}

void UBulletDisplayer::RefreshBullets()
{
	if (Bullets.IsEmpty())
	{
		DebugHelper::LogMessage(10, FColor::Cyan, "No avaiable bullets at void UBulletDisplayer::RefreshBullets()");
		return;
	}

	for (auto& Bullet : Bullets)
	{
		Bullet->Refresh();
	}
}

void UBulletDisplayer::RemoveBullet()
{
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	//AICC_Player* Player = Cast<AICC_Player>(Controller->GetPawn());
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	const AICC_Player* Player = Instance->GetCurrentPlayer();

	UCircularBulletBuffer* Buffer = Player->GetBattleHUD()->GetCircularBulletBuffer();
	
	UBulletData* RemovedBullet = Buffer->RemoveBullet();
	WastedBullets.Add(RemovedBullet);

	if (!RemovedBullet)
	{
		return;
	}
	
	const EBulletType BulletType = RemovedBullet->Type;

	if (Instance->GetInventory().BulletsStored.Contains(BulletType))
	{
		FBullet& BulletStruct = Instance->GetInventory().BulletsStored[BulletType];
		const int32 NewQuantity = FMath::Max(0, BulletStruct.GetQuantity() - 1);
		BulletStruct.SetQuantity(NewQuantity);

		if (NewQuantity == 0)
		{
			Instance->GetInventory().BulletsStored.Remove(BulletType);
		}
	}

	const TArray<UMagazineBullet*>& MagazineBullets = Player->GetBattleHUD()->MagazineBullets;
	const int32 NumSlots = MagazineBullets.Num();
    
	for (int32 i = 0; i < NumSlots; ++i)
	{
		UMagazineBullet* MagazineSlot = MagazineBullets[i];
		const int32 BufferIndex = (Buffer->GetTailIndex() + i) % Buffer->GetCapacity();
		UBulletData* BulletData = (i < Buffer->GetCount()) ? Buffer->PeekAt(BufferIndex) : nullptr;
		MagazineSlot->SetEnableRemoval(BulletData != nullptr);
		MagazineSlot->Setup(Buffer, BulletData, BufferIndex);
	}

	if (BulletType == FearEV || BulletType == JoyEv || BulletType == CalmEV || BulletType == AngerEv)
	{
		return;
	}

	AMob* Target = Cast<AMob>(Player->GetBattleHUD()->GetSelectedActor());
	const float Damage = Instance->GetCurrentDamageData()->CalculateDamage(true);
	Target->GetStats().Health -= Damage;
	Target->GetHealthBar()->SetCurrentHealth(Target->GetStats().Health);

	DebugHelper::AddMessageToLog(
		"Minigame modifier post engage -> " + FString::SanitizeFloat(Instance->GetPlayerStats()->MinigameModifier));

	Player->GetBattleHUD()->RefreshPistolMagazine();
}

UGridPanel* UBulletDisplayer::GetBulletGrid() const
{
	return BulletGrid;
}

UButton* UBulletDisplayer::GetBulletConfirmGamepad() const
{
	return ConfirmGamepadBtn;
}

TArray<UBulletData*> UBulletDisplayer::GetWastedBullets() const
{
	return WastedBullets;
}

void UBulletDisplayer::InstantiateBullet(TArray<FBullet> InstantiateBullets)
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());


	for (FBullet& b : InstantiateBullets)
	{
		b.SetQuantity(100);
		Instance->GetInventory().BulletsStored.Add(b.GetBulletData()->Type, b);
	}

	auto* SpawnManager = Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetEnemySpawnManager();
	const auto Turn = Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().
	                            Queue;

	SpawnManager->GetMemory().Load(Turn, Instance->GetInventory().BulletsStored);
}
