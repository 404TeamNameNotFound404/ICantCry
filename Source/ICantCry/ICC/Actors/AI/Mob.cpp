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
	Damage.BulletData = nullptr;
	Damage.EnemyData = EnemyData;
	Damage.PlayerStats = MinigameHandler->GetBattlePlayer()->GetStats();
	Damage.AIMoves = Moves;
	Handler = MinigameHandler;
	Memory.DefaultBattleLocation = GetActorLocation();
	Memory.DefaultBattleOrientation = GetActorRotation();
	Memory.InitialAttackPower = GetData()->AttackPower;
	Memory.InitialDefencePower = GetData()->DefencePower;
	EnemyData->Alive = true;
	EnemyData->Type = Type;
	Stats.Health = GetData()->MaxHealth;
	Stats.bAlive = true;


	// init unlock note & emotion
	SetDefaultNoteForMobType();
    SetDefaultEmotionForMobType();
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

void AMob::DealDamage()
{
	const float DamageDealt = Damage.CalculateDamage(false);
	const AICC_Player* Player = Handler->GetBattlePlayer();
	Player->GetStats()->CurrentHealth -= DamageDealt;
	Player->GetStats()->CurrentHealth = FMath::Clamp(Player->GetStats()->CurrentHealth, 0.0f,
	                                                 Player->GetStats()->MaxHealth);
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

		// unlock emotion & note on dead
		UnlockNotesOnDeath();
        UnlockEmotionsOnDeath();

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

void AMob::UnlockNotesOnDeath()
{
    if (!Instance)
    {
        UE_LOG(LogTemp, Error, TEXT("[MOB] Cannot unlock notes - GameInstance is null"));
        return;
    }

    if (NotesToUnlockOnDeath.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MOB] No notes to unlock for this mob"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MOB] Attempting to unlock %d notes"), NotesToUnlockOnDeath.Num());
    
    int32 NewlyUnlocked = 0;
    for (const FString& NoteKey : NotesToUnlockOnDeath)
    {
        if (!Instance->CollectedNotes.Contains(NoteKey))
        {
            Instance->CollectedNotes.Add(NoteKey);
            NewlyUnlocked++;
            UE_LOG(LogTemp, Warning, TEXT("[MOB] ✓ Unlocked note: %s"), *NoteKey);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[MOB] ⚠ Note already unlocked: %s"), *NoteKey);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[MOB] Unlocked %d new notes"), NewlyUnlocked);
    UpdateBestiaryUI();
}

void AMob::UnlockEmotionsOnDeath()
{
    if (!Instance)
    {
        UE_LOG(LogTemp, Error, TEXT("[MOB] Cannot unlock emotions - GameInstance is null"));
        return;
    }

    if (EmotionsToUnlockOnDeath.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MOB] No emotions to unlock for this mob"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MOB] Attempting to unlock %d emotions"), EmotionsToUnlockOnDeath.Num());
    
    int32 NewlyUnlocked = 0;
    for (EEmotionType Emotion : EmotionsToUnlockOnDeath)
    {
        if (Emotion != EEmotionType::Default && !Instance->UnlockedEmotions.Contains(Emotion))
        {
            Instance->UnlockedEmotions.Add(Emotion);
            NewlyUnlocked++;
            UE_LOG(LogTemp, Warning, TEXT("[MOB] ✓ Unlocked emotion: %s"), 
                   *UEnum::GetValueAsString(Emotion));
        }
        else if (Emotion == EEmotionType::Default)
        {
            UE_LOG(LogTemp, Warning, TEXT("[MOB] ⚠ Skipped default emotion"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[MOB] ⚠ Emotion already unlocked: %s"), 
                   *UEnum::GetValueAsString(Emotion));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[MOB] Unlocked %d new emotions"), NewlyUnlocked);
    UpdateBestiaryUI();
}

void AMob::UpdateBestiaryUI()
{
    if (!Instance) return;

    // Usa il riferimento diretto dal GameInstance
    Instance->UpdateBestiaryUI();
    
    UE_LOG(LogTemp, Warning, TEXT("[MOB] Requested BestiaryUI update via GameInstance"));
}

void AMob::SetDefaultNoteForMobType()
{
    if (NotesToUnlockOnDeath.Num() == 0)
    {
        // Usa i valori corretti dell'enum EMobType
        switch (Type)
        {
            case EMobType::MobAnger:
                NotesToUnlockOnDeath.Add(TEXT("Anger_Note"));
                break;
            case EMobType::MobFear:
                NotesToUnlockOnDeath.Add(TEXT("Fear_Note"));
                break;
            case EMobType::MobSadness:
                NotesToUnlockOnDeath.Add(TEXT("Sadness_Note"));
                break;
            case EMobType::MobJoy:
                NotesToUnlockOnDeath.Add(TEXT("Joy_Note"));
                break;
            case EMobType::MobDisgust:
                NotesToUnlockOnDeath.Add(TEXT("Disgust_Note"));
                break;
            case EMobType::MobAnxiety:
                NotesToUnlockOnDeath.Add(TEXT("Anxiety_Note"));
                break;
            case EMobType::MobCalm:
                NotesToUnlockOnDeath.Add(TEXT("Calm_Note"));
                break;
            case EMobType::MobJealousy:
                NotesToUnlockOnDeath.Add(TEXT("Jealousy_Note"));
                break;
            case EMobType::MobShame:
                NotesToUnlockOnDeath.Add(TEXT("Shame_Note"));
                break;
            default:
                NotesToUnlockOnDeath.Add(TEXT("Default_Note"));
                break;
        }
    }
}

void AMob::SetDefaultEmotionForMobType()
{
     if (EmotionsToUnlockOnDeath.Num() == 0)
    {
        // Usa i valori corretti dell'enum EMobType
        switch (Type)
        {
            case EMobType::MobAnger:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Anger);
                break;
            case EMobType::MobFear:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Fear);
                break;
            case EMobType::MobSadness:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Sadness);
                break;
            case EMobType::MobJoy:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Joy);
                break;
            case EMobType::MobDisgust:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Disgust);
                break;
            case EMobType::MobAnxiety:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Anxiety);
                break;
            case EMobType::MobCalm:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Calm);
                break;
            case EMobType::MobJealousy:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Jealousy);
                break;
            case EMobType::MobShame:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Shame);
                break;
            default:
                EmotionsToUnlockOnDeath.Add(EEmotionType::Default);
                break;
        }
    }
}

void AMob::UnlockSpecificNote(const FString& NoteKey)
{
    if (!Instance || NoteKey.IsEmpty()) return;
    
    if (!Instance->CollectedNotes.Contains(NoteKey))
    {
        Instance->CollectedNotes.Add(NoteKey);
        UpdateBestiaryUI();
        UE_LOG(LogTemp, Warning, TEXT("[MOB] Manually unlocked note: %s"), *NoteKey);
    }
}

void AMob::UnlockSpecificEmotion(EEmotionType Emotion)
{
    if (!Instance || Emotion == EEmotionType::Default) return;
    
    if (!Instance->UnlockedEmotions.Contains(Emotion))
    {
        Instance->UnlockedEmotions.Add(Emotion);
        UpdateBestiaryUI();
        UE_LOG(LogTemp, Warning, TEXT("[MOB] Manually unlocked emotion: %s"), 
               *UEnum::GetValueAsString(Emotion));
    }
}

bool AMob::HasEmotionUnlocked(EEmotionType Emotion) const
{
    if (!Instance) return false;
    return Instance->UnlockedEmotions.Contains(Emotion);
}

bool AMob::HasNoteUnlocked(const FString& NoteKey) const
{
    if (!Instance) return false;
    return Instance->CollectedNotes.Contains(NoteKey);
}

void AMob::AddNoteToUnlockList(const FString& NoteKey)
{
    if (!NoteKey.IsEmpty() && !NotesToUnlockOnDeath.Contains(NoteKey))
    {
        NotesToUnlockOnDeath.Add(NoteKey);
    }
}

void AMob::AddEmotionToUnlockList(EEmotionType Emotion)
{
    if (Emotion != EEmotionType::Default && !EmotionsToUnlockOnDeath.Contains(Emotion))
    {
        EmotionsToUnlockOnDeath.Add(Emotion);
    }
}
