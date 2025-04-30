// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PathfindingEditorTarget : TargetRules
{
	public PathfindingEditorTarget( TargetInfo Target) : base(Target)
	{
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            bOverrideBuildEnvironment = true;
            AdditionalCompilerArguments = "-Wno-error -Wno-deprecated-builtins";
        }
 
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Pathfinding" } );
	}
}
