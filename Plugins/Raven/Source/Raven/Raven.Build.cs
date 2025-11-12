// RavenStorm Copyright @ 2025-2025

using UnrealBuildTool;

public class Raven : ModuleRules
{
	public Raven(ReadOnlyTargetRules target)
		: base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange([
			"Core",
			"DeveloperSettings",
		]);


		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
		]);
	}
}