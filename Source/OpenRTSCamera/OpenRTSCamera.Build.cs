// Copyright 2024 Jesus Bracho All Rights Reserved.

using UnrealBuildTool;

public class OpenRTSCamera : ModuleRules
{
	public OpenRTSCamera(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", 
				"EnhancedInput",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"Slate",
				"SlateCore",
				"UMG",
				"InputCore",
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
