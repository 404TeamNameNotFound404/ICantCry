// Fill out your copyright notice in the Description page of Project Settings.
#include "Mob.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

FDamage AMob::Damage;
static AMinigameHandler* Handler;

// Sets default values
AMob::AMob()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Enemy");

	HealthBarComponent = CreateDefaultSubobject<UHealthBarWidgetCmp>("HealthBarCmp");
	HealthBarComponent->SetAbsolute(false, false, false);
}

// Called when the game starts or when spawned
void AMob::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AMinigameHandler> It(GetWorld()); It; ++It)
	{
		MinigameHandler = *It;
		break;
	}

	Damage.BulletData = nullptr;
	Damage.EnemyData = EnemyData;
	Damage.PlayerStats = MinigameHandler->GetBattlePlayer()->GetStats();
	Damage.AIMoves = Moves;
	Handler = MinigameHandler;
	DebugHelper::LogSuccess("Move power moves -> " + FString::FromInt(Moves->MovePower));
	DebugHelper::LogSuccess("Move power -> " + FString::FromInt(Damage.AIMoves->MovePower));
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

