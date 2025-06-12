using UnrealBuildTool;

public class TowerOffenseTests : ModuleRules
{
    public TowerOffenseTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "TowerOffense"
        });
    }
}
