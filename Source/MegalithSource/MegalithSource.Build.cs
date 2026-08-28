using UnrealBuildTool;

public class MegalithSource : ModuleRules
{
	public MegalithSource(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"MegalithCore",
			"SQLiteCore",
			"EditorSubsystem",
			"UnrealEd",
			"Json",
			"JsonUtilities",
			"Slate",
			"SlateCore",
			"Projects"
		});
	}
}
