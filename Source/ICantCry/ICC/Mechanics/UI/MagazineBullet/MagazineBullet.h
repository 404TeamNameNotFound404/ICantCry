// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Actors/Bullet/CircularBulletBuffer.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "MagazineBullet.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UMagazineBullet : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UCircularBulletBuffer* InBuffer,  UBulletData* BulletData, int32 Index);
	UFUNCTION() void RemoveFromMagazine();
	
	UButton* GetMagazineBulletButton() const;
	void SetBufferIndex(const int32& NewIndex);
	
protected:
	virtual void NativeConstruct() override;
	UPROPERTY(meta=(BindWidget)) UButton* BulletBtn;
	UPROPERTY() int32 BulletIndex;

private:
	UPROPERTY() UCircularBulletBuffer* Buffer = nullptr;

	UPROPERTY() UICantCryGameInstance* Instance = nullptr;

	UPROPERTY() UBulletData* BulletDataPtr = nullptr;
};
