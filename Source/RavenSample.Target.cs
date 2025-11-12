// RavenStorm Copyright @ 2025-2025

using UnrealBuildTool;

public class RavenSampleTarget : TargetRules
{
	public RavenSampleTarget(TargetInfo target)
		: base(target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("RavenSample");
	}
}