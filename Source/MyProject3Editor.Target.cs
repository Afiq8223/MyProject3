using UnrealBuildTool;

public class MyProject3EditorTarget : TargetRules
{
    public MyProject3EditorTarget(TargetInfo target) : base(target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("MyProject3");
    }
}
