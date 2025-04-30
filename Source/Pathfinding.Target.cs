// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PathfindingTarget : TargetRules
{
	public PathfindingTarget( TargetInfo Target) : base(Target)
	{
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            bOverrideBuildEnvironment = true;
            AdditionalCompilerArguments = "-Wno-error -Wno-deprecated-builtins";
        }
 
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Pathfinding" } );
	}
}
