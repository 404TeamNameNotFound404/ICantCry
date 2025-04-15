#include "HealthBarWidgetCmp.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


UHealthBarWidgetCmp::UHealthBarWidgetCmp()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UHealthBarWidgetCmp::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<AMob>(GetOwner());
	checkf(Owner, TEXT("Owner is null at void UHealthBarWidgetCmp::BeginPlay"));
	HealthBar = Cast<UMobHealthBar>(GetWidget());
	HealthBar->Info = Owner->GetData();
	HealthBar->FillData();

	DebugHelper::LogWarning("Data health -> " + FString::FromInt(HealthBar->GetTotalHealth()));
	DebugHelper::LogWarning("Data Current -> " + FString::FromInt(HealthBar->GetCurrentHealth()));
}



void UHealthBarWidgetCmp::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

