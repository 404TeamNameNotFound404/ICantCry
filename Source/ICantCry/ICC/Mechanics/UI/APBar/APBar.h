// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "APBar.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UAPBar : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY() int32 CurrentAp = 0;
	UPROPERTY() int32 MaxAp = 4;
	UPROPERTY() int32 PreviewAp = 0;
	UPROPERTY() int32 SlotIndex = -1;


public:
	void Init();
	void IncreaseAP(const int32& Value);
	void DecreaseAP(const int32& Value);
	void Update();
	void UpdateHighlights();
	void UpdateHighlights(const int32& Value);
	void ResetHighlightOf(const int32& Value); 
	void SetPreviewAP(const int32& Value);
	void SetCurrentAP(const int32& Value);
	void UpdateAp(const int32& Value, const int32& ApCurrent);
	void ClearAt(const int32& Value);
	void ClearAll();
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget)) UImage* Slot0;
	UPROPERTY(meta = (BindWidget)) UImage* Slot1;
	UPROPERTY(meta = (BindWidget)) UImage* Slot2;
	UPROPERTY(meta = (BindWidget)) UImage* Slot3;

	UPROPERTY(meta = (BindWidget)) UImage* ApHighlight_0;
	UPROPERTY(meta = (BindWidget)) UImage* ApHighlight_1;
	UPROPERTY(meta = (BindWidget)) UImage* ApHighlight_2;
	UPROPERTY(meta = (BindWidget)) UImage* ApHighlight_3;
	
	UPROPERTY() TArray<UImage*> ApBar;
	UPROPERTY() TArray<UImage*> SlotsOutline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "AP System" ,meta=(AllowPrivateAccess=true))
	FColor DefaultColor = FColor::White;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "AP System" ,meta=(AllowPrivateAccess=true))
	FColor ApColor = FColor::Yellow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "AP System" ,meta=(AllowPrivateAccess=true))
	FColor WastedColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "AP System" ,meta=(AllowPrivateAccess=true))
	FColor SlotOutlineColor = FColor::Black;
};
