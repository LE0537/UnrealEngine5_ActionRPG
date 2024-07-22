// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class test2 : ModuleRules
{
	public test2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HairStrandsCore", "Niagara", "GeometryCollectionEngine", "UMG", "AIModule", "LevelSequence", "MovieScene", "CinematicCamera" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PrivateIncludePaths.Add("test2");
        PublicIncludePaths.Add("test2");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
