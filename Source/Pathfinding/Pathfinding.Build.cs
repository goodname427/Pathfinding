// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Pathfinding : ModuleRules
{
	public Pathfinding(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		PublicIncludePaths.AddRange(new string[] 
		{ 
			Path.Combine(ModuleDirectory), 
			Path.Combine(ModuleDirectory, "Gameplay"), 
			Path.Combine(ModuleDirectory, "Pawn"), 
			Path.Combine(ModuleDirectory, "Network"), 
			Path.Combine(ModuleDirectory, "Widget") 
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		
	}
}
