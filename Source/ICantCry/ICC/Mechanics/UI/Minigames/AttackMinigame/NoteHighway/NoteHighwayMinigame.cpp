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
	DebugHelper::LogSuccess("Pressing " + GetNoteName(Target));
	
	for (FHighwayNote& Note : NotesData)
	{
		if (Note.bHit)
		{
			continue;
		}
		
		if (Note.Row != Target)
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


UTexture2D* UNoteHighwayMinigame::LoadProperTexture(FHighwaySpawnable& Spawnable)
{
	switch (Spawnable.ButtonType)
	{
	case A:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return Spawnable.NoteTexture = Icons["Pad_A"];
			}
			
			return Spawnable.NoteTexture = Icons["Key_D"];
		}
	case X:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return Spawnable.NoteTexture = Icons["Pad_X"];
			}
			
			return Spawnable.NoteTexture = Icons["Key_A"];
		}
	case B:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return Spawnable.NoteTexture = Icons["Pad_B"];
			}
			
			return Spawnable.NoteTexture = Icons["Key_S"];
		}
	case Y:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return Spawnable.NoteTexture = Icons["Pad_Y"];
			}
			
			return Spawnable.NoteTexture = Icons["Key_W"];
		}
	default:
		break;
	}
	
	return nullptr;
}

void UNoteHighwayMinigame::SpawnButtons(const ESpawnableHighwayBtn& Type)
{
	for (FHighwaySpawnable& Spawnable : Spawnables)
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
			
			Spawnable.NoteTexture = LoadProperTexture(Spawnable);
			const FVector2D TextureSize = {static_cast<double>(Spawnable.NoteTexture->GetSizeX()), static_cast<double>(Spawnable.NoteTexture->GetSizeY())};
			
			ImgToSpawn->SetBrushFromTexture(Spawnable.NoteTexture);
			ImgToSpawn->SetDesiredSizeOverride(TextureSize);
			
			Parent->AddChild(ImgToSpawn);
			FHighwayNote Note;
			Note.X = Cast<UCanvasPanelSlot>(ImgToSpawn->Slot)->GetPosition().X;
			Note.bHit = false;
			Note.Row = Spawnable.ButtonType;
			Note.CachedTexture = Spawnable.NoteTexture;
			Notes.Add(ImgToSpawn);
			
			NotesData.Add(Note);
		}
	}
}

FString UNoteHighwayMinigame::GetNoteName(const ESpawnableHighwayBtn& Btn) const
{
	switch (Btn)
	{
	case A:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return "A";
			}
			return "D";
		}
	case X:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return "X";
			}
			
			return "A";
		}
	case B:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return "B";
			}
			
			return "S";
		}
	case Y:
		{
			if (DebugHelper::IsGamepadPlugged())
			{
				return "Y";
			}
			
			return "W";
		}
	default:
		return "";
	}
}
