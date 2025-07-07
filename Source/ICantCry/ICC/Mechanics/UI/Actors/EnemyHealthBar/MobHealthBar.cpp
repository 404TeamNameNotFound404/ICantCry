#include "MobHealthBar.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UMobHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMobHealthBar::FillData()
{
	TotalHealth = Info->Health;
	CurrentHealth = TotalHealth;
	HealthBar->SetPercent(CurrentHealth);
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
	Info->Health = FMath::Clamp(Info->Health, 0.0f, Info->MaxHealth);
	const float Percentage = Info->Health / Info->MaxHealth;
	HealthBar->SetPercent(Percentage);
}

void UMobHealthBar::Restore(const float& RestoredHealth)
{
	Info->Health += RestoredHealth;
	const float Percentage = Info->Health / TotalHealth;
	HealthBar->SetPercent(Percentage);
}

bool UMobHealthBar::IsLow() const
{
	return Info->Health <= Info->MaxHealth * 0.2f;
}

