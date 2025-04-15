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
}

