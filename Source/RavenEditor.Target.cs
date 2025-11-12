// RavenStorm Copyright @ 2025-2025

using UnrealBuildTool;

public class RavenEditorTarget : TargetRules
{
	public RavenEditorTarget(TargetInfo target)
		: base(target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("Raven");
	}
}