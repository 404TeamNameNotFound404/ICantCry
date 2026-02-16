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
	const bool Collided = CheckCollision();
	
	return EMinigameThreshold::Bad;
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
	
}

bool UNoteHighwayMinigame::CheckCollision() const
{
	const FVector2D SliderPosition = Slider->GetRenderTransform().Translation;
	
	for (const UImage* Img : Notes)
	{
		if (!Img){continue;}
		
		const UCanvasPanelSlot* ImgSlot = Cast<UCanvasPanelSlot>(Img->Slot);
		
		if (!ImgSlot) continue;
		
		const FVector2D Size = ImgSlot->GetSize();
		
		if (const FVector2D Pos = ImgSlot->GetPosition(); 
			SliderPosition.X >= Pos.X && SliderPosition.X <= Pos.X + Size.X &&
			SliderPosition.Y >= Pos.Y && SliderPosition.Y <= Pos.Y + Size.Y)
		{
			DebugHelper::LogMessage(9, FColor::Blue, "Hit!");
			return true;
		}
	}
	
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
			
			Parent->AddChild(ImgToSpawn);
			Notes.Add(ImgToSpawn);
		}
	}
}
