using UnrealBuildTool;

public class MegalithAudioRuntime : ModuleRules
{
	public MegalithAudioRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Runtime-typed module — ships in packaged builds. NO editor deps.
		// Sub-module of Megalith (sibling to MegalithAudio). Authority-gated MakeNoise
		// requires AIModule + Engine. AssetUserData lives in CoreUObject/Engine.
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AIModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
