using UnrealBuildTool;

public class MegalithLevelSequence : ModuleRules
{
	public MegalithLevelSequence(ReadOnlyTargetRules Target) : base(Target)
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
			"MegalithIndex",
			"SQLiteCore",
			"UnrealEd",
			"AssetRegistry",
			"MovieScene",
			"MovieSceneTracks",
			"LevelSequence",
			"BlueprintGraph",
			"Kismet",
			"EditorSubsystem",
			"Json",
			"JsonUtilities"
		});
	}
}
