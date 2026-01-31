// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldSlot.h"


// Sets default values
AFieldSlot::AFieldSlot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

int32 AFieldSlot::GetId() const
{
	return Id;
}

// Called when the game starts or when spawned
void AFieldSlot::BeginPlay()
{
	Super::BeginPlay();
	
}

