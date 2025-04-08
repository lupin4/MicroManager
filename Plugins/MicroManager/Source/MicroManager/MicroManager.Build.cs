using UnrealBuildTool;

public class MicroManager : ModuleRules
{
	public MicroManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// Add public include paths required here
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				// Add private include paths required here
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd", // Required for editor functionalities
				"Blutility",
				"EditorScriptingUtilities",// Required for Blueprint utility classes
				"Slate", "SlateCore", "UMG", "Niagara", "CinematicCamera", "MovieScene",
				"MovieSceneTracks", "LevelSequence","AssetRegistry",
				"AssetTools",
				"ContentBrowser","InputCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				// Add private dependencies that you statically link with here
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// Add any modules that your module loads dynamically here
			}
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorStyle",
					// Add other editor-specific modules here
				}
			);
		}
	}
}