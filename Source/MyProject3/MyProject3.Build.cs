using UnrealBuildTool;

public class MyProject3 : ModuleRules
{
    public MyProject3(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        PrivateDependencyModuleNames.AddRange(new[] { "InputCore", "Slate", "SlateCore", "UMG" });
        RuntimeDependencies.Add("$(ProjectDir)/Content/Station/Fonts/NotoSansTamil.ttf", StagedFileType.UFS);
        RuntimeDependencies.Add("$(ProjectDir)/Content/Station/Fonts/NotoSansSC.ttf", StagedFileType.UFS);
        RuntimeDependencies.Add("$(ProjectDir)/Content/Station/Fonts/NotoSansTamil-OFL.txt", StagedFileType.UFS);
        RuntimeDependencies.Add("$(ProjectDir)/Content/Station/Fonts/NotoSansSC-OFL.txt", StagedFileType.UFS);
    }
}
