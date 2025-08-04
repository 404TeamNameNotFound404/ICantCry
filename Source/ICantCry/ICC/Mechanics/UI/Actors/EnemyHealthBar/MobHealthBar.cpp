#include "MobHealthBar.h"

#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UMobHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}


void UMobHealthBar::FillData2(AMob* Mob)
{
	TotalHealth = Info->MaxHealth;
	CurrentHealth = Mob->GetStats().Health;
	Owner = Mob;
}

float UMobHealthBar::GetTotalHealth() const
{
	return TotalHealth;
}

float UMobHealthBar::GetCurrentHealth() const
{
	return CurrentHealth;
}

void UMobHealthBar::SetCurrentHealth(const float& Variation)
{
	CurrentHealth = Variation;
	//Info->Health = FMath::Clamp(Info->Health, 0.0f, Info->MaxHealth);
	//const float Percentage = Info->Health / Info->MaxHealth;
	Owner->GetStats().Health = FMath::Clamp(Owner->GetStats().Health, 0.0f, Info->MaxHealth);
	const float Percentage = Owner->GetStats().Health / Info->MaxHealth;
	HealthBar->SetPercent(Percentage);
}

void UMobHealthBar::Restore(const float& RestoredHealth)
{
	// Info->Health += RestoredHealth;
	// const float Percentage = Info->Health / TotalHealth;

	Owner->GetStats().Health += RestoredHealth;
	const float Percentage = Owner->GetStats().Health / TotalHealth;
	HealthBar->SetPercent(Percentage);
}

bool UMobHealthBar::IsLow() const
{
	//return Info->Health <= Info->MaxHealth * 0.2f;
	return Owner->GetStats().Health <= Owner->GetStats().Health * 0.2f;
}

