// Fill out your copyright notice in the Description page of Project Settings.
#include "IccBlueprintDebug.h"
#include "DebugHelper.h"

void UIccBlueprintDebug::EmitLog()
{
	DebugHelper::SaveLogToFile();
	DebugHelper::LogSuccess("Log saved correctly");
}
