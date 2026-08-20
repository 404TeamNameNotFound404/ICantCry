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
	FHighwayNote* TargetNote = FindClosestNote(Target);
	
	if (!TargetNote) 
	{
		DebugHelper::LogWarning("Pressed with no note in range.");
		return;
	}
	
	const FGeometry& SliderGeo = Slider->GetTickSpaceGeometry();
	const float SliderX = SliderGeo.GetAbsolutePosition().X + (SliderGeo.GetAbsoluteSize().X * 0.5f);

	const FGeometry& NoteGeo = TargetNote->CachedSelf->GetTickSpaceGeometry();
	const float NoteX = NoteGeo.GetAbsolutePosition().X + (NoteGeo.GetAbsoluteSize().X * 0.5f);

	const float Distance = FMath::Abs(SliderX - NoteX);
    
	const float PerfectWindow = HitTolerance; 
	const float EarlyPenalizeWindow = HitTolerance * 2.5f; 
    
	if (Distance <= PerfectWindow)
	{
		TargetNote->bHit = true;
		Score = FMath::Min(Score + 0.20f, 1.5f);
        
		const FString IconKey = DebugHelper::IsGamepadPlugged() ? TEXT("OPad_") : TEXT("OKey_");
		if (UTexture2D** FoundTexture = Icons.Find(FName(*(IconKey + GetNoteName(Target)))))
		{
			TargetNote->CachedSelf->SetBrushFromTexture(*FoundTexture);
		}
       
		DebugHelper::LogSuccess("Note hit perfectly!");
		DisplayFeedback(true);
	}
	else if (NoteX > SliderX && Distance <= EarlyPenalizeWindow)
	{
		TargetNote->bHit = true; 
		//Score = FMath::Max(Score - 0.15f, 0.0f); 
		TargetNote->CachedSelf->SetColorAndOpacity(FLinearColor::Red);
       
		DebugHelper::LogWarning("Hit too early!");
		DisplayFeedback(false);
	}
	else 
	{
		TargetNote->bHit = true; 
		//Score = FMath::Max(Score - 0.25f, 0.0f);
		TargetNote->CachedSelf->SetColorAndOpacity(FLinearColor::Red);
       
		DebugHelper::LogError("Spam press / Missed completely!");
		DisplayFeedback(false);
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
	HideFeedback();
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
	
	const FGeometry& SliderGeo = Slider->GetTickSpaceGeometry();
	const float SliderCenterX = SliderGeo.GetAbsolutePosition().X + (SliderGeo.GetAbsoluteSize().X * 0.5f);

	const float MaxSearchDistance = HitTolerance * 3.5f;

	for (FHighwayNote& Note : NotesData)
	{
		if (Note.bHit || Note.Row != Type || !Note.CachedSelf) continue;
		
		const FGeometry& NoteGeo = Note.CachedSelf->GetTickSpaceGeometry();
		const float NoteCenterX = NoteGeo.GetAbsolutePosition().X + (NoteGeo.GetAbsoluteSize().X * 0.5f);
		
		if (const float Distance = FMath::Abs(SliderCenterX - NoteCenterX);
			Distance <= MaxSearchDistance && Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestNote = &Note;
		}
	}
	
	return ClosestNote;
}

void UNoteHighwayMinigame::DisplayFeedback(const bool& bIsNice)
{
	
	if (UTexture2D* TargetTexture = bIsNice ? NiceFeedback : MissFeedback;
		TargetTexture)
	{
		FeedbackImg->SetBrushFromTexture(TargetTexture);
	}
	
	if (UCanvasPanelSlot* FeedbackSlot = Cast<UCanvasPanelSlot>(FeedbackImg->Slot))
	{
		if (const UWidget* ParentWidget = FeedbackImg->GetParent())
		{
			FeedbackSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			FeedbackSlot->SetAlignment(FVector2D(0.5f, 0.5f));

			const FGeometry& ParentGeo = ParentWidget->GetTickSpaceGeometry();
			const FGeometry& SliderGeo = Slider->GetTickSpaceGeometry();
       
			const float SliderAbsoluteCenterX = SliderGeo.GetAbsolutePosition().X + (SliderGeo.GetAbsoluteSize().X * 0.5f);
			const FVector2D LocalPos = ParentGeo.AbsoluteToLocal(FVector2D(SliderAbsoluteCenterX, 0.0f));
       
			const float SliderPosY = Slider->GetRenderTransform().Translation.Y;
			
			FeedbackSlot->SetPosition(FVector2D(LocalPos.X + FeedbackXOffset, SliderPosY - FeedbackYOffset));
		}
	}
	
	FeedbackImg->SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(FeedbackTimer, this, &UNoteHighwayMinigame::HideFeedback, 0.4f, false);
}

void UNoteHighwayMinigame::HideFeedback()
{
	FeedbackImg->SetVisibility(ESlateVisibility::Hidden);
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
