// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../BulletBottonItem.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "StandardBulletDisplayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UStandardBulletDisplayer : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* AngerDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* AngerEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* FearDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* FearEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* DisgustDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* DisgustEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* SadnessDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* SadnessEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JoyDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JoyEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* CalmDv;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* CalmEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JealousyDV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* JealousyEV;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Anxiety;
	UPROPERTY(meta = (BindWidget)) UBulletBottonItem* Shame;

public:
	TArray<UBulletBottonItem*> GetBullet() const;
	
	void Init(UInventoryHUD* Hud); // calls to set the OwnerHUD of each bulletbottomitem
	void Unlock(ERecipeType Type);
	void Refresh();


private:
	UPROPERTY()
	TArray<UBulletBottonItem*> StandardBullet;

	UPROPERTY()
	bool bBound = false;

	UPROPERTY()
	UICantCryGameInstance* GameInstance;
	
	
};
