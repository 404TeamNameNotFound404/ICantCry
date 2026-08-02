// Fill out your copyright notice in the Description page of Project Settings.


#include "IccAnimationDealer.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

UIccAnimationDealer::UIccAnimationDealer()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UIccAnimationDealer::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}


void UIccAnimationDealer::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	if (Owner->bIsInFight) return;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector Velocity = Owner->GetVelocity();
	Velocity.Z = 0.0f;
	
	const float CurrentSpeed = Velocity.Size();
	const float MaxSpeed = MoveCmp->GetMaxSpeed();
	
	const float TargetBlending = (MaxSpeed > 0.0f) ? FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f) : 0.0f;
	constexpr float InterpSpeed = 10.0f;
	
	AnimationInstance->LocomotionBlending = FMath::FInterpTo(
		AnimationInstance->LocomotionBlending, TargetBlending, DeltaTime, InterpSpeed);
	
}

UIccPlayerAnimInstance* UIccAnimationDealer::GetAnimationBlueprint()
{
	return AnimationInstance;
}

void UIccAnimationDealer::SetInFight(const bool& InFight)
{
	AnimationInstance->bInFight = InFight;
}

void UIccAnimationDealer::SetShooting(const bool& InShooting)
{
	AnimationInstance->bShooting = InShooting;
	
	FTimerHandle GoBackDefault;
	GetWorld()->GetTimerManager().SetTimer(
			GoBackDefault, 
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				AnimationInstance->bShooting = false;
			}),
			0.15f,
			false
		);
}

void UIccAnimationDealer::Init()
{
	Owner = Cast<AICC_Player>(GetOwner());
	if (!Owner) return;
	
	const USkeletalMeshComponent* Cmp = Owner->GetMesh();
	if (!Cmp) return;
	
	AnimationInstance = Cast<UIccPlayerAnimInstance>(Cmp->GetAnimInstance());
	
	if (AnimationInstance)
	{
		DebugHelper::LogMessage(5, FColor::Purple, 
			"Dealer and animation blueprint configured correctly");
	}
	
	
	MoveCmp = Owner->GetMovementComponent();
}

