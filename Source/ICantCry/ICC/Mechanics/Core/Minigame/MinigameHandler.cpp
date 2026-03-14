// Fill out your copyright notice in the Description page of Project Settings.
#include "MinigameHandler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Blueprint/UserWidget.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/Defense Minigame//DefenceMinigame.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/AttackMinigame/AttackMinigame.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/AttackMinigame/AngerMinigame/AngerAtkMinigame.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/AttackMinigame/Curling/CurlingMinigame.h"
#include "ICantCry/ICC/Mechanics/UI/Minigames/AttackMinigame/NoteHighway/NoteHighwayMinigame.h"


// Sets default values
AMinigameHandler::AMinigameHandler() : AttackMinigame(nullptr), DefenseMinigame(nullptr) , CurrentMinigameDisplayed(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AMinigameHandler::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AICC_Player> It(GetWorld()); It; ++It)
	{
		Player = *It;
		break;
	}
}

void AMinigameHandler::StartMinigame(const bool& EnableAttack)
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	checkf(Controller, TEXT("Controller is null at AMinigameHandler::StartMinigame"));
	
	
	if (EnableAttack)
	{
		// start attack minigame by default
		CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, AttackMinigame);
		UAttackMinigame* CastedWidget = Cast<UAttackMinigame>(CurrentMinigameDisplayed);

		if (!CurrentMinigameDisplayed)
		{
			DebugHelper::LogError("Couldn't create minigame, something is null");
			return;
		}
		
		//CurrentMinigameDisplayed->AddToViewport();
		Player->GetBattleHUD()->MinigameSlot->AddChild(CurrentMinigameDisplayed);
		Player->GetBattleHUD()->HideInfo();
		Player->EnableMinigameInput(true);
		Player->SetActiveMinigameUserWidget(CastedWidget);
		CurrentMinigameDisplayed->SetKeyboardFocus();
		bPlayerMinigameEnded = false;
	}

	
	//otherwise call defence minigame 
	if (!EnableAttack)
	{
		CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, DefenseMinigame);
		UDefenceMinigame* CastedWidget = Cast<UDefenceMinigame>(CurrentMinigameDisplayed);
		
		if (!CurrentMinigameDisplayed)
		{
			DebugHelper::LogError("Couldn't create minigame, something is null");
			return;
		}
		
		//CurrentMinigameDisplayed->AddToViewport();
		Player->GetBattleHUD()->MinigameSlot->AddChild(CurrentMinigameDisplayed);
		Player->GetBattleHUD()->HideInfo();
		Player->EnableMinigameInput(true);
		Player->SetActiveMinigameUserWidget(CastedWidget);
		CurrentMinigameDisplayed->SetKeyboardFocus();
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameHasStarted(true);
		
		AMob::SetMinigameStarted(true);
		AMob::MinigameEnded = false;
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameEnd(false);
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetIsBusy(true);
	}
}

void AMinigameHandler::StartMinigame(UBulletData* BulletData, const bool& EnableAttack)
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	checkf(Controller, TEXT("Controller is null at AMinigameHandler::StartMinigame"));
	
	if (EnableAttack)
	{
		UMinigameUserWidget* CastedWidget = nullptr;
		
		switch (BulletData->MinigameTemplate)
		{
		default:
		case Default:
			{
				CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, AttackMinigame);
				CastedWidget = Cast<UAttackMinigame>(CurrentMinigameDisplayed);
				break;
			}
		case Anger:
			{
				CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["Anger"]);
				CastedWidget = Cast<UAngerAtkMinigame>(CurrentMinigameDisplayed);
				break;
			}
		case GuitarHero:
			{
				switch (BulletData->Type)
				{
				default:
				case Indifference:
				case AngerDv:
				case Shame:
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["GuitarHero 4"]);
					CastedWidget = Cast<UNoteHighwayMinigame>(CurrentMinigameDisplayed);

				
				case FearDv:
				case Disgust:
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["GuitarHero 2"]);
					CastedWidget = Cast<UNoteHighwayMinigame>(CurrentMinigameDisplayed);
					break;
				case Anxiety:
					DebugHelper::LogMessage(10, FColor::White, "Guitar hero spawned");
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["GuitarHero 1"]);
					CastedWidget = Cast<UNoteHighwayMinigame>(CurrentMinigameDisplayed);
					break;
				case SadnessDv:
					break;
				case JoyDv:
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["GuitarHero 3"]);
					CastedWidget = Cast<UNoteHighwayMinigame>(CurrentMinigameDisplayed);
					break;
					
				case JoyEv:
				case CalmEV:
				case AngerEv:
				case FearEV:
				case SadnessEv:
				case JealousyEv:
				case DisgustEv:
				case CalmDv:
					break;
				case JealousyDv:
					break;
				}
				break;
			}
		case Curling:
			{
				
				switch (BulletData->Type)
				{
				default:
				case Indifference:
				case AngerDv:
				case AngerEv:
				case FearEV:
				case JoyEv:
				case CalmEV:
				case Shame:
				case JealousyEv:
				case DisgustEv:
				case SadnessEv:
				case FearDv:
				case Disgust:
					break;
				case Anxiety:
					break;
				case SadnessDv:
					break;
				case JoyDv:
					break;
				case CalmDv:
					DebugHelper::LogMessage(10, FColor::White, "Curling spawned");
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["Curling 1"]);
					CastedWidget = Cast<UCurlingMinigame>(CurrentMinigameDisplayed);
					break;
				case JealousyDv:
					CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, MinigameClasses["Curling 2"]);
					CastedWidget = Cast<UCurlingMinigame>(CurrentMinigameDisplayed);
					break;
				}
				break;
			}
		}

		
		if (!CurrentMinigameDisplayed)
		{
			DebugHelper::LogError("Couldn't create minigame, something is null");
			return;
		}
		
		UGameplayStatics::PlaySound2D(this, CtvFx);
		
		FTimerHandle Delay;
		
		Player->GetBattleHUD()->CTR->SetVisibility(ESlateVisibility::Visible);
	
		GetWorld()->GetTimerManager().SetTimer(Delay, [this, CastedWidget]()
		{
			CurrentMinigameDisplayed->AddToViewport();
			Player->GetBattleHUD()->MinigameSlot->AddChild(CurrentMinigameDisplayed);
			Player->GetBattleHUD()->HideInfo();
			Player->EnableMinigameInput(true);
			Player->SetActiveMinigameUserWidget(CastedWidget);
			CurrentMinigameDisplayed->SetKeyboardFocus();
			bPlayerMinigameEnded = false;
			Player->GetBattleHUD()->CTR->SetVisibility(ESlateVisibility::Hidden);
		}, 1.f , false);
	}

	
	//otherwise call defence minigame 
	if (!EnableAttack)
	{
		CurrentMinigameDisplayed = CreateWidget<UUserWidget>(Controller, DefenseMinigame);
		UDefenceMinigame* CastedWidget = Cast<UDefenceMinigame>(CurrentMinigameDisplayed);
		
		if (!CurrentMinigameDisplayed)
		{
			DebugHelper::LogError("Couldn't create minigame, something is null");
			return;
		}
		
		//CurrentMinigameDisplayed->AddToViewport();
		Player->GetBattleHUD()->MinigameSlot->AddChild(CurrentMinigameDisplayed);
		Player->GetBattleHUD()->HideInfo();
		Player->EnableMinigameInput(true);
		Player->SetActiveMinigameUserWidget(CastedWidget);
		CurrentMinigameDisplayed->SetKeyboardFocus();
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameHasStarted(true);
		
		AMob::SetMinigameStarted(true);
		AMob::MinigameEnded = false;
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameEnd(false);
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetIsBusy(true);
	}
}

void AMinigameHandler::EndMinigame()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	checkf(Controller, TEXT("Controller is null at AMinigameHandler::StartMinigame"));
	checkf(CurrentMinigameDisplayed, TEXT("Current minigame displayed is null at AMinigameHandler::EndMinigame"));

	// just remove the current minigame from the viewport and make it null so it will be ready to display the new one
	// it disables also

	CurrentMinigameDisplayed->RemoveFromParent();
	CurrentMinigameDisplayed = nullptr;
	// FInputModeGameOnly InputMode;
	// Controller->SetInputMode(InputMode);
	Controller->bShowMouseCursor = true;
	Player->EnableMinigameInput(false);

	AMob::SetMinigameStarted(false);
	AMob::MinigameEnded = true;

	if (Player->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetIsPlayerTurn())
	{
		// Player->GetBattleHUD()->GetSelectedEmotion()->SetMinigameHasStarted(false);
		// Player->GetBattleHUD()->GetSelectedEmotion()->SetMinigameEnd(true);
		Cast<AMob>(Player->GetBattleHUD()->GetSelectedActor())->SetMinigameHasStarted(false);
		Cast<AMob>(Player->GetBattleHUD()->GetSelectedActor())->SetMinigameEnd(true);
		bPlayerMinigameEnded = true;
		
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
		
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->Fire(Instance->GetCurrentPlayer()->GetBattleHUD()->GetSelectedActor()->GetActorLocation(), 
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetCurrentBulletData()->DisplayColor);
	}
	
	if (Player->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->GetIsAITurn())
	{
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameHasStarted(false);
		Player->GetBattleHUD()->GetCurrentPlayingEmotion()->SetMinigameEnd(true);
		bPlayerMinigameEnded = true;
	}
	
	bPlayerMinigameEnded = true;

	OnMinigameEnded.Broadcast();
}

AICC_Player* AMinigameHandler::GetBattlePlayer() const
{
	return Player;
}

bool AMinigameHandler::IsPlayerMinigameEnded() const
{
	return bPlayerMinigameEnded;
}


