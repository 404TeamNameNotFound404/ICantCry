// Fill out your copyright notice in the Description page of Project Settings.
#include "Mob.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

bool AMob::MinigameEnded = false;
bool AMob::bMinigameHasStarted = false;
bool AMob::bStopTree = false;
UICantCryGameInstance* AMob::GameRef;
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
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AICC_AIController::StaticClass();
}

// Called when the game starts or when spawned
void AMob::BeginPlay()
{
	Super::BeginPlay();
	bIsReady = true;
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

	GameRef = Instance;
	Handler = MinigameHandler;
	Memory.DefaultBattleLocation = GetActorLocation();
	Memory.DefaultBattleOrientation = GetActorRotation();
	Memory.InitialAttackPower = GetData()->RuntimeStats.AtkPower;
	Memory.InitialDefencePower = GetData()->RuntimeStats.DefPower;

	DebugHelper::AddMessageToLog("[AMob]: Memory registered InitialAttackPower as: " + FString::SanitizeFloat(Memory.InitialAttackPower));
	DebugHelper::AddMessageToLog("[AMob]: Memory registered InitialDefPower as: " + FString::SanitizeFloat(Memory.InitialDefencePower));
	
	EnemyData->Alive = true;
	EnemyData->Type = Type;
	Stats.Health = GetData()->MaxHealth;
	Stats.bAlive = true;
	
	RefDamage = FDamage(nullptr,Instance->GetPlayerStats(), Moves, EnemyData, this, Instance);
}

// Called every frame
void AMob::Tick(float DeltaTime)
{
	if (!IsAlive())
	{
		return;
	}

	Super::Tick(DeltaTime);
}

int AMob::GetSpeed() const
{
	return Moves->Priority;
}

UEnemyDatas* AMob::GetData() const
{
	return EnemyData;
}

UEnemyTactics* AMob::GetTactics() const
{
	return Moves;
}

UBattleData* AMob::GetBattleData() const
{
	return BattleData;
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

void AMob::SetIsRespawned(const bool& Value)
{
	bRespawned = Value;
	bIsReady = false;
}

bool AMob::IsRespawned() const
{
	return bRespawned;
}

void AMob::ReinizializeTree()
{
	if (!AIController)
	{
		DebugHelper::LogMessage(10, FColor::Red, "AI controller is invalid at AMob::ReinizializeTree()");
		return;
	}
	
	AIController->GetBlackboardComponent()->SetValueAsObject("Target", DebugPlayer);
	AIController->GetBlackboardComponent()->SetValueAsObject("SelfActor", this);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffed?", bIsBuffedAtk);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsDefenceDebuffed?", bIsDebuffedDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsDefenceBuffed?", bBuffDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsShieldDebuffed?", bDebuffShield);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsOtherShieldDebuffed?", bDebuffOtherShield);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffedOtherDef?", bBuffOtherDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAlive?", GetStats().bAlive);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsEnvyBurnedState?", bEnvyBurned);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAshamedState?", bIsAshamedState);
	AIController->GetBlackboardComponent()->SetValueAsBool("Attacked?", bAttacked);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsFreezedUp?", bFreezedUp);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAttackDebuffed?", bIsAttackDebuffed);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffOtherAtk?", bBuffOtherAtk);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsHealing?", bHeal);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsHealingOther?", bHealOther);
	bIsReady = true;
	bRespawned = false;
	DebugHelper::LogWarning("Emotion BT re-initialized!");
	DebugHelper::AddMessageToLog("Emotion BT re-initialized!");
}

void AMob::StartDefenceMinigame()
{
	MinigameHandler->StartMinigame(false);
}

bool AMob::IsHealer() const
{
	return bIsHealer;
}

void AMob::SetIsBuffedAtk(const bool& Value)
{
	bIsBuffedAtk = Value;
}

void AMob::SetIsTargetDefDebuffed(const bool& Value)
{
	bIsDebuffedDefence = Value;
}

void AMob::SetIsAshamedState(const bool& Value)
{
	bIsAshamedState = Value;
}

void AMob::SetIsLow(const bool& Value)
{
	bIsLow = Value;
}

void AMob::SetIsFreezedUp(const bool& Value)
{
	bFreezedUp = Value;
}

void AMob::SetIsAttacked(const bool& Value)
{
	bAttacked = Value;
}

void AMob::SetIsEnvyBurned(const bool& Value)
{
	bEnvyBurned = Value;
}

void AMob::SetTreeId(const int& Value)
{
	Bt_Id = Value;
}

void AMob::SetDebuffShield(const bool& Value)
{
	bDebuffShield = Value;
}

void AMob::SetDebuffOtherShield(const bool& Value)
{
	bDebuffOtherShield = Value;
}

void AMob::SetHeal(const bool& Value)
{
	bHeal = Value;
}

void AMob::SetHealOther(const bool& Value)
{
	bHealOther = Value;
}

void AMob::SetBuffedDefence(const bool& Value)
{
	bBuffDefence = Value;
}

void AMob::SetBuffOtherDefence(const bool& Value)
{
	bBuffOtherDefence = Value;
}

void AMob::SetBuffOtherAtk(const bool& Value)
{
	bBuffOtherAtk = Value;
}

void AMob::SetPlayerDebuffAttack(const bool& Value)
{
	bIsAttackDebuffed = Value;
}

bool AMob::GetIsIsBuffedAtk() const
{
	return bIsBuffedAtk;
}

bool AMob::GetIsTargetDefenceDebuffed() const
{
	return bIsDebuffedDefence;
}

bool AMob::GetIsIsAshamedState() const
{
	return bIsAshamedState;
}

bool AMob::GetIsIsLow() const
{
	return bIsLow;
}

bool AMob::GetIsIsFreezedUp() const
{
	return bFreezedUp;
}

bool AMob::GetIsIsAttacked() const
{
	return bAttacked;
}

bool AMob::GetIsIsEnvyBurned() const
{
	return bEnvyBurned;
}

bool AMob::GetIsDebuffShield() const
{
	return bDebuffShield;
}

bool AMob::GetIsDebuffOtherShield() const
{
	return bDebuffOtherShield;
}

bool AMob::GetIsHeal() const
{
	return bHeal;
}

bool AMob::GetIsHealOther() const
{
	return bHealOther;
}

bool AMob::GetIsBuffedDefence() const
{
	return bBuffDefence;
}

bool AMob::GetBuffOtherAtk() const
{
	return bBuffOtherAtk;
}

bool AMob::GetPlayerDebuffAttack() const
{
	return bIsAttackDebuffed;
}

bool AMob::GetBuffOtherDefence() const
{
	return bBuffOtherDefence;
}

int AMob::GetTreeId() const
{
	return Bt_Id;
}

bool AMob::GetRethink() const
{
	return bRethink;
}

void AMob::SetRethink(const bool& Value)
{
	bRethink = Value;
}

bool AMob::IsFleeing() const
{
	return bFlee;
}

void AMob::SetFleeing(const bool& Value)
{
	bFlee = Value;
}

void AMob::Flee()
{
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	
	AIController = Cast<AICC_AIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Flee"); 
	}
}

void AMob::UnlockContentOnDeath()
{
	 if (!Instance)
    {
        UE_LOG(LogTemp, Error, TEXT("[MOB] Cannot unlock content - GameInstance is null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MOB] Unlocking content for mob type: %s"), 
           *UEnum::GetValueAsString(GetMobType()));

    // Sblocca note in base al MobType
    FString NoteKey = GetNoteKeyForMobType();
    if (!NoteKey.IsEmpty() && !Instance->CollectedNotes.Contains(NoteKey))
    {
        Instance->CollectedNotes.Add(NoteKey);
        UE_LOG(LogTemp, Warning, TEXT("[MOB] ✓ Unlocked note: %s"), *NoteKey);
    }

    // Sblocca emotion in base al MobType
    EEmotionType EmotionToUnlock = GetEmotionForMobType();
    if (EmotionToUnlock != EEmotionType::Default && !Instance->UnlockedEmotions.Contains(EmotionToUnlock))
    {
        Instance->UnlockedEmotions.Add(EmotionToUnlock);
        UE_LOG(LogTemp, Warning, TEXT("[MOB] ✓ Unlocked emotion: %s"), 
               *UEnum::GetValueAsString(EmotionToUnlock));
    }

    // Aggiorna Bestiary UI
    Instance->UpdateBestiaryUI();
}

UDecisionTable* AMob::GetDecisionTable() const
{
	return DecisionTable;
}

bool AMob::IsLowHealth() const
{
	constexpr float Threshold = 0.2f;
	return Stats.Health <= GetData()->MaxHealth * Threshold;
}

FString AMob::GetNoteKeyForMobType() const
{
    switch (Type)
    {
        case EMobType::MobAnger:    return TEXT("Anger");
        case EMobType::MobFear:     return TEXT("Fear");
        case EMobType::MobSadness:  return TEXT("Sadness");
        case EMobType::MobJoy:      return TEXT("Joy");
        case EMobType::MobDisgust:  return TEXT("Disgust");
        case EMobType::MobAnxiety:  return TEXT("Anxiety");
        case EMobType::MobCalm:     return TEXT("Calm");
        case EMobType::MobJealousy: return TEXT("Jealousy");
        case EMobType::MobShame:    return TEXT("Shame");
        default:                    return TEXT("Default");
    }
}

EEmotionType AMob::GetEmotionForMobType() const
{
    switch (Type)
    {
        case EMobType::MobAnger:    return EEmotionType::Anger;
        case EMobType::MobFear:     return EEmotionType::Fear;
        case EMobType::MobSadness:  return EEmotionType::Sadness;
        case EMobType::MobJoy:      return EEmotionType::Joy;
        case EMobType::MobDisgust:  return EEmotionType::Disgust;
        case EMobType::MobAnxiety:  return EEmotionType::Anxiety;
        case EMobType::MobCalm:     return EEmotionType::Calm;
        case EMobType::MobJealousy: return EEmotionType::Jealousy;
        case EMobType::MobShame:    return EEmotionType::Shame;
        default:                    return EEmotionType::Default;
    }
}


void AMob::Heal(const float& RestoredHealth)
{
	if (!bIsHealer)
	{
		return;
	}

	UMobHealthBar* Bar = Cast<UMobHealthBar>(HealthBarComponent->GetWidget());
	checkf(Bar, TEXT("Bar is invalid in AMob::Heal"));
	Bar->Restore(RestoredHealth);
}

UMobHealthBar* AMob::GetHealthBar() const
{
	UMobHealthBar* Bar = Cast<UMobHealthBar>(HealthBarComponent->GetWidget());
	checkf(Bar, TEXT("Bar is invalid in AMob::GetHealthBar"));
	return Bar;
}

bool AMob::IsEAnger() const
{
	return bEAnger;
}

bool AMob::IsEFear() const
{
	return bIsEFear;
}

bool AMob::IsEDisgust() const
{
	return bIsEDisgust;
}

bool AMob::IsESadness() const
{
	return bIsESadness;
}

bool AMob::IsEAnxiety() const
{
	return bIsEAnxiety;
}

bool AMob::IsECalm() const
{
	return bIsECalm;
}

bool AMob::IsEJealousy() const
{
	return bIsEJealousy;
}

bool AMob::IsEShame() const
{
	return bIsEShame;
}



void AMob::InflictDamage()
{
	const float DamageDealt = RefDamage.CalculateDamage(false);
	AICC_Player* Player = Handler->GetBattlePlayer();
	Player->GetRuntimeStats().CurrentHealth -= DamageDealt;
	Player->GetRuntimeStats().CurrentHealth = FMath::Clamp(Player->GetRuntimeStats().CurrentHealth, 0.0f, Player->GetStats()->MaxHealth);
	const float HealthPercentage = Player->GetRuntimeStats().CurrentHealth / Player->GetStats()->MaxHealth;
	Player->GetBattleHUD()->PlayerHealth->SetPercent(HealthPercentage);
	Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->Flow();
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
	if (bStopTree || IsFreezed())
	{
		return;
	}

	// For some reason after the merge the 'DebugPlayer' couldn't be found, so I just iterate again if it does not find it at the 'BeginPlay'
	if (!DebugPlayer)
	{
		for (TActorIterator<AICC_Player> It(GetWorld()); It; ++It)
		{
			DebugPlayer = *It;
			break;
		}
	}

	bIsBuffedAtk = false;
	bIsDebuffedDefence = false;
	bIsLow = false;
	bFreezedUp = false;
	bAttacked = false;
	bEnvyBurned = false;
	bIsAshamedState = false;
	bDebuffShield = false;
	bBuffOtherAtk = false;
	bBuffOtherDefence = false;
	bHeal = false;
	bHealOther = false;
	Bt_Id = 0;
	bRethink = false;
	bFlee = false;
	
	AIController = Cast<AICC_AIController>(GetController());
	checkf(AIController, TEXT("AI Controller is invalid at AMob::PlayTurn"));

	AIController->GetBlackboardComponent()->SetValueAsObject("Target", DebugPlayer);
	AIController->GetBlackboardComponent()->SetValueAsObject("SelfActor", this);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffed?", bIsBuffedAtk);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsDefenceDebuffed?", bIsDebuffedDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsDefenceBuffed?", bBuffDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsShieldDebuffed?", bDebuffShield);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsOtherShieldDebuffed?", bDebuffOtherShield);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffedOtherDef?", bBuffOtherDefence);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAlive?", GetStats().bAlive);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsEnvyBurnedState?", bEnvyBurned);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAshamedState?", bIsAshamedState);
	AIController->GetBlackboardComponent()->SetValueAsBool("Attacked?", bAttacked);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsFreezedUp?", bFreezedUp);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsAttackDebuffed?", bIsAttackDebuffed);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsBuffOtherAtk?", bBuffOtherAtk);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsHealing?", bHeal);
	AIController->GetBlackboardComponent()->SetValueAsBool("IsHealingOther?", bHealOther);
	AIController->GetBlackboardComponent()->SetValueAsBool("Rethinker", bRethink);
	AIController->GetBlackboardComponent()->SetValueAsBool("Flee?", bFlee);

	GetWorld()->GetTimerManager().SetTimer(BehaviorTreeTimerHandle, [this]()
	{
		AIController->RunBehaviorTree(Tree);
	}, FMath::RandRange(0.25f, 0.3f), false);
}

void AMob::SetIsReadyToPlay(const bool& Value)
{
	bIsReady = Value;
}

bool AMob::IsAIReadyToPlay() const
{
	 return bIsReady;
}


bool AMob::IsAlive()
{
	if (Stats.Health <= 0)
	{
		bIsReady = false;
		Stats.bAlive = false;

		if (!bHasUnlockedContent)
        {
            UnlockContentOnDeath();
            bHasUnlockedContent = true;
        }


		// Destroy();
		SetActorTickEnabled(false);
		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
		AIController = Cast<AICC_AIController>(GetController());
		AIController->GetBrainComponent()->StopLogic("Death");
		return false;
	}

	Stats.bAlive = true;
	return true;
}

void AMob::EndTurn()
{
	AIController = Cast<AICC_AIController>(GetController());
	checkf(AIController, TEXT("AI Controller is invalid at AMob::PlayTurn"));
	AIController->BrainComponent->StopLogic("End Turn");
}


FEmotionStat& AMob::GetStats()
{
	return Stats;
}

EMobType AMob::GetMobType() const
{
	return Type;
}


bool AMob::IsMinigameStarted()
{
	return bMinigameHasStarted;
}

bool AMob::IsMinigameHasStarted() const
{
	return bMinigameStarted;
}

bool AMob::IsMinigameEnded() const
{
	return bMinigameEnded;
}

void AMob::SetMinigameEnd(const bool& Value)
{
	bMinigameEnded = Value;
}

void AMob::SetMinigameStarted(const bool& Value)
{
	bMinigameHasStarted = Value;
}


void AMob::SetMinigameHasStarted(const bool& Value)
{
	bMinigameStarted = Value;
}

ABattleHandler* AMob::GetBattleHandler() const
{
	checkf(BattleHandler, TEXT("Battle Handler is invalid at AMob::GetBattleHandler"));
	return BattleHandler;
}


void AMob::UpdateBestiaryUI()
{
    if (!Instance) return;

    // Usa il riferimento diretto dal GameInstance
    Instance->UpdateBestiaryUI();
    
    UE_LOG(LogTemp, Warning, TEXT("[MOB] Requested BestiaryUI update via GameInstance"));
}


