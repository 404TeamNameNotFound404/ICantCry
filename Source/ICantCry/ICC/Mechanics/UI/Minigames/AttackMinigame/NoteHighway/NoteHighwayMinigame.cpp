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
		HandleScore();
		Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
		Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
		Instance->GetCurrentPlayer()->GetBinder()->SetDecreaseMinigameScrollValue(false);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->Fire(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor()->GetActorLocation(), 
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->DisplayColor);
		Instance->GetCurrentPlayer()->GetMinigameHandler()->EndMinigame();
	}
}

EMinigameThreshold UNoteHighwayMinigame::CheckBar()
{
	return UMinigameUserWidget::CheckBar();
}

void UNoteHighwayMinigame::Simulate(const ESpawnableHighwayBtn& Target)
{
	const UCanvasPanelSlot* SliderSlot = Cast<UCanvasPanelSlot>(Slider->Slot);
	if (!SliderSlot) return;
	const float SliderX = Slider->GetTickSpaceGeometry().GetAbsolutePosition().X;

	for (FHighwayNote& Note : NotesData)
	{
		if (Note.bHit || Note.Row != Target)
			continue;

		const float NoteX = Note.CachedSelf->GetTickSpaceGeometry().GetAbsolutePosition().X;
		
		if (const float Distance = FMath::Abs(SliderX - NoteX);
			Distance <= HitTolerance)
		{
			Note.bHit = true;

			const FString IconName = GetNoteName(Target);
			const FString Prefix = DebugHelper::IsGamepadPlugged() ? TEXT("OPad_") : TEXT("OKey_");
			const FName FinalMapKey = FName(*(Prefix + IconName));

			if (UTexture2D** FoundTexture = Icons.Find(FinalMapKey))
			{
				Score = FMath::Min(Score + 0.25f, 1.5f);
				Note.CachedSelf->SetBrushFromTexture(*FoundTexture);
			}

			DebugHelper::LogSuccess("Note hit!");
			break;
		}
	}
}

void UNoteHighwayMinigame::NativeConstruct()
{
	Super::NativeConstruct();
	
	Notes.Empty();
	NotesData.Empty();
	
	EndBorder->SetVisibility(ESlateVisibility::Hidden);
	
	for (FHighwaySpawnable& Spawnable : Spawnables)
	{
		SpawnButtons(Spawnable);
	}
	
	DebugHelper::LogMessage(8, FColor::White, "Notes " + FString::FromInt( Notes.Num()));
}

void UNoteHighwayMinigame::HandleScore()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	DebugHelper::LogMessage(6, FColor::White, "Total Score: " + FString::SanitizeFloat(Score));
	Instance->GetRuntimeStats().MinigameModifier = Score;
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->UpdateAp();
	Instance->GetCurrentPlayer()->GetBattleHUD()->EnableButtonsAfterShooting();
	Instance->GetCurrentPlayer()->GetBattleHUD()->ApDecreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	Instance->GetCurrentPlayer()->GetBattleHUD()->ApIncreaseOnShoot->SetVisibility(ESlateVisibility::Hidden);
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBulletDisplayer()->RemoveBullet();
	Instance->GetPlayerStats()->RuntimeStats.ApModifier = 0;
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

void UNoteHighwayMinigame::SpawnButtons(FHighwaySpawnable& Spawnable)
{
	UCanvasPanel* Parent = Cast<UCanvasPanel>(GetWidgetFromName(Spawnable.RowName));
	if (!Parent) return;

	UImage* ImgToSpawn = NewObject<UImage>(this, UImage::StaticClass());
	if (!ImgToSpawn) return;

	Spawnable.NoteTexture = LoadProperTexture(Spawnable);
	const FVector2D TextureSize = { (double)Spawnable.NoteTexture->GetSizeX(), (double)Spawnable.NoteTexture->GetSizeY() };

	ImgToSpawn->SetBrushFromTexture(Spawnable.NoteTexture);
	ImgToSpawn->SetDesiredSizeOverride(TextureSize);

	if (UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(Parent->AddChild(ImgToSpawn)))
	{
		ImageSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		ImageSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ImageSlot->SetPosition(FVector2D::ZeroVector);
		ImageSlot->SetSize(TextureSize);
		FHighwayNote Note;
		Note.X = ImgToSpawn->GetTickSpaceGeometry().GetAbsolutePosition().X;
		Note.Row = Spawnable.ButtonType;
		Note.bHit = false;
		Note.CachedSelf = ImgToSpawn;
		
		NotesData.Add(Note);
		Notes.Add(ImgToSpawn);
	}
	ImgToSpawn->ForceLayoutPrepass();
}

void UNoteHighwayMinigame::Reset()
{
	if (NotesData.IsEmpty()) return;
	if (Notes.IsEmpty()) return;
	
	for (UImage* NoteImg : Notes)
	{
		if (NoteImg)
		{
			NoteImg->RemoveFromParent();
		}
	}
	
	Notes.Empty();
	NotesData.Empty();
	Score = 0.5f;
	
	if (UCanvasPanelSlot* SliderSlot = Cast<UCanvasPanelSlot>(Slider->Slot))
	{
		SliderSlot->SetPosition(FVector2D(0, 0));
	}
}

FHighwayNote* UNoteHighwayMinigame::FindClosestNote(const ESpawnableHighwayBtn& Type)
{
	FHighwayNote* ClosestNote = nullptr;
	float MinDistance = FLT_MAX;

	const float SliderX = Slider->GetRenderTransform().Translation.X;

	for (FHighwayNote& Note : NotesData)
	{
		if (Note.bHit)
			continue;

		if (Note.Row != Type)
			continue;
		

		if (const float Distance = FMath::Abs(SliderX - Note.X); Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestNote = &Note;
		}
	}

	return ClosestNote;
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
