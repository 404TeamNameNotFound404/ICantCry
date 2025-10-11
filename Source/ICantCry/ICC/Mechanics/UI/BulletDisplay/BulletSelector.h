// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Actors/Bullet/Bullet.h"
#include "ICantCry/ICC/Mechanics/UI/MagazineBullet/MagazineBullet.h"
#include "BulletSelector.generated.h"


class AICC_Player;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBulletSelector : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(FBullet& NewBullet, int32 InQuantity);

	UFUNCTION() void DisplayBulletInfo();

	//debug only
	FBullet& GetBullet();

	FBullet* GetBulletPtr();

	void Refresh();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UButton* Bullet;

	UPROPERTY(meta=(BindWidget))
	UImage* BulletIcon;

	UFUNCTION()
	void AddToRevolver();

	UPROPERTY()
	AICC_Player* Player;

	UPROPERTY()
	FBullet BulletRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true)) TSubclassOf<UMagazineBullet> BulletClass;
	
	FBullet* BulletRefPtr = nullptr;
	
};
