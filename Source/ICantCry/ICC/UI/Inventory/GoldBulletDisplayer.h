// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../BulletBottonItem.h"
#include "GoldBulletDisplayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UGoldBulletDisplayer : public UUserWidget
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Dummy;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Dummy2;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* FearDV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* FearEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* DisgustDV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* DisgustEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* SadnessDV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* SadnessEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JoyDV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JoyEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* None;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* CalmEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JealousyDV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JealousyEV;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Anxiety;
	// UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Shame;

public:
	TArray<UBulletBottonItem *> GetBullet() const;


private:
	UPROPERTY()
	TArray<UBulletBottonItem*> GoldBullet;



	
};

