using UnrealBuildTool;
using System.IO;

public class MegalithLogicDriver : ModuleRules
{
	public MegalithLogicDriver(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Release builds: set MEGALITH_RELEASE_BUILD=1 to force all optional deps off.
		bool bHasLogicDriver = false;
		bool bReleaseBuild = System.Environment.GetEnvironmentVariable("MEGALITH_RELEASE_BUILD") == "1";

		if (!bReleaseBuild)
		{
			// 1. Check project Plugins/ folder
			string ProjectPluginsDir = Path.Combine(
				Target.ProjectFile.Directory.FullName, "Plugins");
			if (Directory.Exists(ProjectPluginsDir))
			{
				bHasLogicDriver = Directory.Exists(
					Path.Combine(ProjectPluginsDir, "SMSystem"))
					|| Directory.GetDirectories(
						ProjectPluginsDir, "LogicDriver*",
						SearchOption.TopDirectoryOnly).Length > 0;
			}

			// 2. Check Engine Plugins/Marketplace/ folder (Fab install)
			if (!bHasLogicDriver)
			{
				string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
				string MarketplaceDir = Path.Combine(EngineDir, "Plugins", "Marketplace");
				if (Directory.Exists(MarketplaceDir))
				{
					bHasLogicDriver = Directory.GetDirectories(
						MarketplaceDir, "LogicDriver*",
						SearchOption.TopDirectoryOnly).Length > 0;
				}

				// 3. Check Engine Plugins/ root
				if (!bHasLogicDriver)
				{
					string EnginePluginsDir = Path.Combine(EngineDir, "Plugins");
					bHasLogicDriver = Directory.Exists(
						Path.Combine(EnginePluginsDir, "SMSystem"));
				}
			}
		}

		if (bHasLogicDriver)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"Core", "CoreUObject", "Engine",
				"MegalithCore", "MegalithIndex",
				"UnrealEd",
				"BlueprintGraph",
				"EditorScriptingUtilities",
				"SMSystem", "SMSystemEditor",
				"GameplayTags",
				"Json", "JsonUtilities"
			});
			PublicDefinitions.Add("WITH_LOGICDRIVER=1");
		}
		else
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"Core", "CoreUObject", "Engine",
				"MegalithCore"
			});
			PublicDefinitions.Add("WITH_LOGICDRIVER=0");
		}
	}
}
