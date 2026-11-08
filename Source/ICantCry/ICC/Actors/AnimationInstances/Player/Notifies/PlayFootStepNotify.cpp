// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayFootStepNotify.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UPlayFootStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	if (!MeshComp)
		return;

	AICC_Player* Player = Cast<AICC_Player>(MeshComp->GetOwner());

	if (!Player) return;

	if (USoundDealer* SoundDealer =
		Player->FindComponentByClass<USoundDealer>())
	{
		//SoundDealer->PlayFootStep();
		SoundDealer->PlayRandomFootSteps();
	}
}
