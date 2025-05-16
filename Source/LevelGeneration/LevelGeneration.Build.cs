using UnrealBuildTool;

public class LevelGeneration : ModuleRules
{
    public LevelGeneration(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GeometryScriptingCore", "NavigationSystem", "GeometryAlgorithms", "GeometryAlgorithms" });
        if (Target.bBuildEditor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "Slate", "SlateCore", "Mechimera" });
        }
    }
}
