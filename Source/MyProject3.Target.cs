using UnrealBuildTool;

public class MyProject3Target : TargetRules
{
    public MyProject3Target(TargetInfo target) : base(target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("MyProject3");
    }
}
