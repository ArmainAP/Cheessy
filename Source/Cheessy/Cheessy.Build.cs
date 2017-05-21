// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Cheessy : ModuleRules
{
	public Cheessy(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "GameSparks", "OnlineSubsystem", "UMG", "Slate", "SlateCore", "Networking", "Sockets"});

        PrivateDependencyModuleNames.AddRange(new string[] { "Http", "Icmp" });
    }
}
