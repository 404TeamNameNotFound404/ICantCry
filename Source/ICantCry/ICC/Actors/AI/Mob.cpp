// Fill out your copyright notice in the Description page of Project Settings.
#include "Mob.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

FDamage AMob::Damage;
bool AMob::MinigameEnded = false;
bool AMob::bMinigameHasStarted = false;
bool AMob::bStopTree = false;
static AMinigameHandler* Handler;

// Sets default values
AMob::AMob()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add("Enemy");

	Shape = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Shape"));
	Shape->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Shape->SetSimulatePhysics(false);
	HealthBarComponent = CreateDefaultSubobject<UHealthBarWidgetCmp>("HealthBarCmp");
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetAbsolute(false, false, false);
	HealthBarComponent->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AMob::BeginPlay()
{
	Super::BeginPlay();
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
	{
		MinigameHandler = *It;
		break;
	}

	for (TActorIterator<AICC_Player> It(GetWorld()); It; ++It)
	{
		DebugPlayer = *It;
		break;
	}

	for (TActorIterator<ABattleHandler> It(GetWorld()); It; ++It)
	{
		BattleHandler = *It;
		break;
	}

	Damage.BulletData = nullptr;
	Damage.EnemyData = EnemyData;
	Damage.PlayerStats = MinigameHandler->GetBattlePlayer()->GetStats();
	Damage.AIMoves = Moves;
	Handler = MinigameHandler;
	Memory.DefaultBattleLocation = GetActorLocation();
	Memory.DefaultBattleOrientation = GetActorRotation();
	EnemyData->Alive = true;
}

// Called every frame
void AMob::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UEnemyDatas *AMob::GetData() const
{
    return EnemyData;
}

UEnemyTactics* AMob::GetTactics() const
{
	return Moves;
}

UBehaviorTree* AMob::GetBehaviorTree() const
{
	if (!Tree)
	{
		return nullptr;
	}
	
	return Tree;
}

FEmotionMemory AMob::GetAIMemory() const
{
	return Memory;
}

void AMob::HighlightsSilhouette()
{
	if (!bEnableSilhouette)
	{
		DebugHelper::AddTurnMaterialOverlayToStaticMesh(StaticMesh);
		bEnableSilhouette = true;
	}
}

void AMob::DisableSilhouette()
{
	DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(StaticMesh);
	bEnableSilhouette = false;
}

void AMob::StartDefenceMinigame()
{
	MinigameHandler->StartMinigame(false);
}

void AMob::DealDamage()
{
	const float DamageDealt = Damage.CalculateDamage(false);
	const AICC_Player* Player = Handler->GetBattlePlayer();
	Player->GetStats()->CurrentHealth -= DamageDealt;
	Player->GetStats()->CurrentHealth = FMath::Clamp(Player->GetStats()->CurrentHealth, 0.0f, Player->GetStats()->MaxHealth);
	const float HealthPercentage = Player->GetStats()->CurrentHealth / Player->GetStats()->MaxHealth;
	Player->GetBattleHUD()->PlayerHealth->SetPercent(HealthPercentage);
	
}

void AMob::SetIsBusy(const bool& Value)
{
	bBusy = Value;
}

bool AMob::IsBusy() const
{
	return bBusy;
}

void AMob::PlayTurn()
{
	if (bStopTree)
	{
		return;
	}
	
	AIController = Cast<AICC_AIController>(GetController());
	checkf(AIController, TEXT("AI Controller is invalid at AMob::PlayTurn"));
	AIController->GetBlackboardComponent()->SetValueAsObject("Target", DebugPlayer);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffed?", false);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsDebuffed?", false);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAlive?", GetData()->Alive);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsEnvyBurnedState?", false);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAshamedState?", false);
	AIController->GetBlackboardComponent()->SetValueAsBool("Attacked?", false);
	AIController->RunBehaviorTree(Tree);
}

void AMob::EndTurn()
{
	AIController = Cast<AICC_AIController>(GetController());
	checkf(AIController, TEXT("AI Controller is invalid at AMob::PlayTurn"));
	AIController->BrainComponent->StopLogic("End Turn");
}



bool AMob::IsMinigameStarted()
{
	return bMinigameHasStarted;
}

void AMob::SetMinigameStarted(const bool& Value)
{
	bMinigameHasStarted = Value;
}

ABattleHandler* AMob::GetBattleHandler() const
{
	return BattleHandler;
}

