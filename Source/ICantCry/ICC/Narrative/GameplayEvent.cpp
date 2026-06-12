// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEvent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


// leaving the base implementation empty
// since this is a blueprintnativeevent, this function will be overridden in blueprints
// or in child c++ classes like event_giverewards
void UGameplayEvent::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	// optional base logic goes here, but it's intentionally left empty
	// derived classes and blueprints will provide the actual implementation
}

