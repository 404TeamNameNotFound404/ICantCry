// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteHighwayMinigame.h"

#include "Components/CanvasPanelSlot.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UNoteHighwayMinigame::MoveSlider(const FVector2D& Position)
{
	FVector2D CurrentPosition = Slider->GetRenderTransform().Translation;
	
	CurrentPosition.Y = 0;
	const FVector2D DeltaMove = Position * SliderSpeed * GetWorld()->GetDeltaSeconds();
	FVector2D NewPosition = CurrentPosition + DeltaMove;
	NewPosition.Y = 0;
	FVector2D BarrierPosition = EndBorder->GetRenderTransform().Translation;
	BarrierPosition.Y = 0;
	
	Slider->SetRenderTranslation(NewPosition);
	
	if (const bool bCollided = CheckCollision(); bCollided){}
	
	if (const float Distance = FVector2D::Distance(BarrierPosition, NewPosition); 
		Distance >= EndThreshold)
	{
		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
		HandleScore();
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
	}
}

EMinigameThreshold UNoteHighwayMinigame::CheckBar()
{
	return UMinigameUserWidget::CheckBar();
}

void UNoteHighwayMinigame::Simulate(const ESpawnableHighwayBtn& Target)
{
	for (FHighwayNote& Note : NotesData)
	{
		if (Note.bHit)
		{
			continue;
		}
		
		if (const float HitDistance = FMath::Abs(Slider->GetRenderTransform().Translation.X - Note.X);
			HitDistance <= HitTolerance)
		{
			Note.bHit = true;
			
			Score = Score + 0.2f;
			
			// TODO Load 'Hit' Texture
			DebugHelper::LogSuccess("Note hit!");
			
			
			
			return;
		}
	}
}

void UNoteHighwayMinigame::NativeConstruct()
{
	Super::NativeConstruct();
	
	EndBorder->SetVisibility(ESlateVisibility::Hidden);
	
	for (FHighwaySpawnable& Spawnable : Spawnables)
	{
		SpawnButtons(Spawnable.ButtonType);
	}
}

void UNoteHighwayMinigame::HandleScore()
{
	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	Instance->GetPlayerStats()->RuntimeStats.MinigameModifier = Score;
	Score = 0;
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
	Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	
}

void UNoteHighwayMinigame::Flow()
{
	
}

bool UNoteHighwayMinigame::CheckCollision()
{
	return false;
}


void UNoteHighwayMinigame::SpawnButtons(const ESpawnableHighwayBtn& Type)
{
	for (const FHighwaySpawnable& Spawnable : Spawnables)
	{
		if (Spawnable.ButtonType == Type)
		{
			UCanvasPanel* Parent = Cast<UCanvasPanel>(GetWidgetFromName(Spawnable.RowName));
			
			if (!Parent)
			{
				continue;
			}
			
			UImage* ImgToSpawn = NewObject<UImage>(this, UImage::StaticClass());
			
			if (!ImgToSpawn)
			{
				continue;
			}
			
			const FVector2D TextureSize = {static_cast<double>(Spawnable.NoteTexture->GetSizeX()), static_cast<double>(Spawnable.NoteTexture->GetSizeY())};
			
			ImgToSpawn->SetBrushFromTexture(Spawnable.NoteTexture);
			ImgToSpawn->SetDesiredSizeOverride(TextureSize);
			
			FHighwayNote Note;
			Note.X = 0;
			Note.bHit = false;
			Note.CachedTexture = Spawnable.NoteTexture;
			
			Parent->AddChild(ImgToSpawn);
			Notes.Add(ImgToSpawn);
			
			NotesData.Add(Note);
		}
	}
}
