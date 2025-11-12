// RavenStorm Copyright @ 2025-2025

using UnrealBuildTool;

public class RavenSample : ModuleRules
{
	public RavenSample(ReadOnlyTargetRules target)
		: base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange([
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		]);
	}
}