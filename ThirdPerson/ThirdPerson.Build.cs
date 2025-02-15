// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdPerson : ModuleRules
{
	public ThirdPerson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","ControlRig","AnimGraphRuntime" });
	     PrivateDependencyModuleNames.AddRange(new string[] {
            "RigVM"       // Add this line
        });
	}
}
