// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PolyPalsDefense : ModuleRules
{
	public PolyPalsDefense(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public"),
            Path.Combine(ModuleDirectory, "Public/Core"),
            Path.Combine(ModuleDirectory, "Public/AssetManagement"),
            Path.Combine(ModuleDirectory, "Public/DataAsset"),
            Path.Combine(ModuleDirectory, "Public/Multiplayer"),
            Path.Combine(ModuleDirectory, "Public/Multiplayer/Components"),
            Path.Combine(ModuleDirectory, "Public/Multiplayer/Components/PolyPalsController"),
            Path.Combine(ModuleDirectory, "Public/Multiplayer/Components/PolyPalsGamePawn"),
            Path.Combine(ModuleDirectory, "Public/Enemy"),
            Path.Combine(ModuleDirectory, "Public/Tower"),
            Path.Combine(ModuleDirectory, "Public/UI"),
            Path.Combine(ModuleDirectory, "Public/Map")
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NetCore", "NavigationSystem", "Niagara", "HTTP" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "OnlineSubsystem", "OnlineSubsystemUtils" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
