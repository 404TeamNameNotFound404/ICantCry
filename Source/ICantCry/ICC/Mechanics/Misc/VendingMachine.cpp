// Fill out your copyright notice in the Description page of Project Settings.


#include "VendingMachine.h"


// Sets default values
AVendingMachine::AVendingMachine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bCanEverTick = false;
	
	VendingUi = CreateDefaultSubobject<UWidgetComponent>(TEXT("VendingUi"));
}

// Called when the game starts or when spawned
void AVendingMachine::BeginPlay()
{
	Super::BeginPlay();
	
	VendingUi->SetWidget(VendingMachineWidget);
}


