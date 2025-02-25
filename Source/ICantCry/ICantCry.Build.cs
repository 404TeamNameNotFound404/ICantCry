// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ICantCry : ModuleRules
{
	public ICantCry(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicDefinitions.Add("USE_LINUX_SPECIFIC_CODE=1");
		}
		else if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("USE_WINDOWS_SPECIFIC_CODE=1");
		}
		
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" , "GameplayTags"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
