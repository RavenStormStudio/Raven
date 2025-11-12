// RavenStorm Copyright @ 2025-2025

using UnrealBuildTool;

public class RavenSampleEditorTarget : TargetRules
{
	public RavenSampleEditorTarget(TargetInfo target)
		: base(target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("RavenSample");
	}
}