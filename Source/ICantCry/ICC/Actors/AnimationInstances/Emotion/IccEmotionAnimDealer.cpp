// Fill out your copyright notice in the Description page of Project Settings.


#include "IccEmotionAnimDealer.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

// Sets default values for this component's properties
UIccEmotionAnimDealer::UIccEmotionAnimDealer()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UIccEmotionAnimDealer::BeginPlay()
{
	Super::BeginPlay();

	Init();
	
}


void UIccEmotionAnimDealer::Init()
{
	Owner = Cast<AMob>(GetOwner());
	if (!Owner) return;
	
	const USkeletalMeshComponent* Cmp = Owner->GetMesh();
	if (!Cmp) return;
	
	AnimationInstance = Cast<UIccEmotionAnimInstance>(Cmp->GetAnimInstance());
	if (AnimationInstance)
	{
		DebugHelper::LogMessage(5, FColor::Orange, 
			"Emotion Animation instance configured correctly");
	}
}

// Called every frame
void UIccEmotionAnimDealer::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

