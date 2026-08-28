using UnrealBuildTool;

public class MegalithMaterial : ModuleRules
{
	public MegalithMaterial(ReadOnlyTargetRules Target) : base(Target)
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
			"UnrealEd",
			"MaterialEditor",
			"EditorScriptingUtilities",
			"AssetTools",
			"RenderCore",
			"RHI",
			"Slate",
			"SlateCore",
			"Json",
			"JsonUtilities"
		});
	}
}
