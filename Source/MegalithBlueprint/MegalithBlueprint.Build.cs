using UnrealBuildTool;

public class MegalithBlueprint : ModuleRules
{
	public MegalithBlueprint(ReadOnlyTargetRules Target) : base(Target)
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
			"BlueprintGraph",
			"BlueprintEditorLibrary",
			"SubobjectDataInterface",
			"Kismet",
			"KismetCompiler",
			"EditorScriptingUtilities",
			"EnhancedInput",
			"Json",
			"JsonUtilities"
			// (Historical: StructUtils was added here by PR #40 but is deprecated
			// since UE 5.5 — FInstancedStruct relocated into CoreUObject and resolves
			// transparently via the existing CoreUObject Public dep above.)
		});
	}
}
