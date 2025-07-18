// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverVisualizer.h"

void UGameOverVisualizer::NativeConstruct()
{
	Super::NativeConstruct();

	Retry->OnClicked.AddDynamic(this, &UGameOverVisualizer::RetryBattle);
	LoadPreviousSave->OnClicked.AddDynamic(this, &UGameOverVisualizer::LoadPrevious);
}

void UGameOverVisualizer::RetryBattle()
{
	// TODO add retry
}

void UGameOverVisualizer::LoadPrevious()
{
	// TODO add load save or checkpoint
}
