// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletSelector.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "Components/HorizontalBox.h"
#include "BulletDisplayer.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBulletDisplayer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UGridPanel* BulletGrid;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<UBulletSelector> BulletButtonItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BulletDisplayer" ,meta=(AllowPrivateAccess=true))
	int32 BulletSlotPadding = 4;

	UPROPERTY()
	TArray<UBulletSelector*> Bullets;

public:
	void Setup();
	
	void Refresh();

	TArray<UBulletSelector*> GetBullets() const;

	void RefreshBullets();

	void RemoveBullet();

	UGridPanel* GetBulletGrid() const;
	
	/**
	 * DEBUG ONLY!
	 */
	UFUNCTION(BlueprintCallable)
	void InstantiateBullet(TArray<FBullet> InstantiateBullets);
};
