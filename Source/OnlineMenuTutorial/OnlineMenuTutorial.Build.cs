// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnlineMenuTutorial : ModuleRules
{
	public OnlineMenuTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"OnlineMenuTutorial",
			"OnlineMenuTutorial/Variant_Platforming",
			"OnlineMenuTutorial/Variant_Platforming/Animation",
			"OnlineMenuTutorial/Variant_Combat",
			"OnlineMenuTutorial/Variant_Combat/AI",
			"OnlineMenuTutorial/Variant_Combat/Animation",
			"OnlineMenuTutorial/Variant_Combat/Gameplay",
			"OnlineMenuTutorial/Variant_Combat/Interfaces",
			"OnlineMenuTutorial/Variant_Combat/UI",
			"OnlineMenuTutorial/Variant_SideScrolling",
			"OnlineMenuTutorial/Variant_SideScrolling/AI",
			"OnlineMenuTutorial/Variant_SideScrolling/Gameplay",
			"OnlineMenuTutorial/Variant_SideScrolling/Interfaces",
			"OnlineMenuTutorial/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"OnlineSubsystem",
			"OnlineSubsystemSteam"
		});

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
