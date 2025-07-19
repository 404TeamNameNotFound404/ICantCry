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

	Main->ClearChildren();       
	Bullets.Empty();
	
	if (Instance->GetInventory().BulletsStored.IsEmpty())
	{
		return;
	}

	for (auto& Bullet : Instance->GetInventory().BulletsStored)
	{
		FBullet& B = Bullet.Value;
		UBulletSelector* Item = CreateWidget<UBulletSelector>(GetWorld(), BulletButtonItemClass);
		Item->Setup(B, B.GetQuantity());
		Item->SetPadding(FMargin(2,2));
		Main->AddChild(Item);
		Bullets.Add(Item);
		DebugHelper::LogWarning("Found " + B.GetBulletData()->BulletName);
	}
}



void UBulletDisplayer::Refresh()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	Main->ClearChildren();              
	Bullets.Empty();

	if (Instance->GetInventory().BulletsStored.IsEmpty())
	{
		DebugHelper::LogWarning("No inventory available");
		return;
	}

	for (auto& Bullet : Instance->GetInventory().BulletsStored)
	{
		
		FBullet& B = Bullet.Value;

		if (B.GetQuantity() <= 0)
		{
			DebugHelper::LogWarning( B.GetBulletData()->BulletName + " is 0");
			break;
		}
		
		UBulletSelector* Item = CreateWidget<UBulletSelector>(GetWorld(), BulletButtonItemClass);
		Item->Setup(B, B.GetQuantity());
		Item->SetPadding(FMargin(2,2));
		Main->AddChild(Item);
		Item->SetIsFocusable(true);
		Bullets.Add(Item);
	}

	DebugHelper::LogWarning("Refreshing avaiable bullets");
}

TArray<UBulletSelector*> UBulletDisplayer::GetBullets() const
{
	return Bullets;
}

void UBulletDisplayer::RefreshBullets()
{
	if (Bullets.IsEmpty())
	{
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
	AICC_Player* Player = Cast<AICC_Player>(Controller->GetPawn());
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	

	UBulletData* RemovedBullet = Player->GetBattleHUD()->GetCircularBulletBuffer()->RemoveBullet();
	EBulletType BulletType = RemovedBullet->Type;
	
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
	
	RefreshBullets();
	
	//AMob* Target = Player->GetBattleHUD()->GetSelectedEmotion();

	if (BulletType == FearEV || BulletType == JoyEv || BulletType == CalmEV || BulletType == AngerEV)
	{
		return;
	}
	
	AMob* Target = Cast<AMob>(Player->GetBattleHUD()->GetSelectedActor());
	const float Damage = Instance->GetCurrentDamageData().CalculateDamage(true);
	// Target->GetData()->Health -= Damage;
	// Target->GetHealthBar()->SetCurrentHealth(Target->GetData()->Health);
	Target->GetStats().Health -= Damage;
	Target->GetHealthBar()->SetCurrentHealth(Target->GetStats().Health);
}

