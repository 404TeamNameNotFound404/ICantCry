// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "NoteHighwayMinigame.generated.h"

UENUM(BlueprintType)
enum ESpawnableHighwayBtn
{
	A, // S
	X, // A
	B, // D
	Y // W
};

USTRUCT(BlueprintType)
struct FHighwaySpawnable
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ESpawnableHighwayBtn> ButtonType;
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetOptional))
	FName RowName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* NoteTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeOffset;
};


USTRUCT(BlueprintType)
struct FHighwayNote
{
	GENERATED_BODY()
	
	UPROPERTY() UTexture2D* CachedTexture;
	UPROPERTY() UImage* CachedSelf;
	UPROPERTY() TEnumAsByte<ESpawnableHighwayBtn> Row;
	UPROPERTY() float X;
	UPROPERTY() float TargetTime;
	UPROPERTY() bool bHit;
	UPROPERTY() float HitTolerance = 50.0f;
};

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UNoteHighwayMinigame : public UMinigameUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void MoveSlider(const FVector2D& Position) override;
	virtual EMinigameThreshold CheckBar() override;
	
	void Simulate(const ESpawnableHighwayBtn& Target);
	
protected:
	virtual void NativeConstruct() override;
	virtual void HandleScore() override;
	virtual void Flow() override;
	
	UPROPERTY(meta=(BindWidget)) UImage* Slider;
	
	// Rows where to spawn the buttons at runtime
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* YRow0;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* YRow1;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* YRow2;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* YRow3;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* XRow0;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* XRow1;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* XRow2;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* XRow3;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* BRow0;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* BRow1;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* BRow2;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* BRow3;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* ARow0;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* ARow1;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* ARow2;
	UPROPERTY(meta=(BindWidget)) UCanvasPanel* ARow3;
	
	UPROPERTY(meta=(BindWidget)) UImage* EndBorder;
	UPROPERTY(meta=(BindWidget)) UImage* FeedbackImg;
	
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Minigames",  meta = (AllowPrivateAccess = "true"))
	TArray<FHighwaySpawnable> Spawnables;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Minigames",  meta = (AllowPrivateAccess = "true"))
	TMap<FName, UTexture2D*> Icons;
	
	UPROPERTY()
	TArray<UImage*> Notes;
	
	UPROPERTY()
	TArray<FHighwayNote> NotesData;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Minigames", meta = (AllowPrivateAccess = "true"))
	float SliderSpeed = 180.f;
	
	UPROPERTY()
	float EndThreshold = 692.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Minigames", meta = (AllowPrivateAccess = "true"))
	float HitTolerance = 42.f;
	
	UPROPERTY() float Score = 0.5f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Minigames", meta = (AllowPrivateAcess = "true"))
	float StartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Minigames", meta=(AllowPrivateAccess = "true"))
	UTexture2D* NiceFeedback;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Minigames", meta=(AllowPrivateAccess = "true"))
	UTexture2D* MissFeedback;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Minigames", meta=(AllowPrivateAccess = "true"))
	float FeedbackXOffset = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Minigames", meta=(AllowPrivateAccess = "true"))
	float FeedbackYOffset = 40.f;
	
	FTimerHandle FeedbackTimer;
	
	UTexture2D* LoadProperTexture(FHighwaySpawnable& Spawnable);
	
	//void SpawnButtons(const ESpawnableHighwayBtn& Type);
	void SpawnButtons(FHighwaySpawnable& Spawnable);
	
	void Reset();
	
	FHighwayNote* FindClosestNote(const ESpawnableHighwayBtn& Type);
	
	void DisplayFeedback(const bool& bIsNice);
	void HideFeedback();
	
	FString GetNoteName(const ESpawnableHighwayBtn& Btn) const;
	
};
