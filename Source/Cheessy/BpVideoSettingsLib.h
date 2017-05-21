// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BpVideoSettingsLib.generated.h"

/**
 * Aceasta clasa este o librarie statica pentru sistemul de Visual Scripting utilizat de Unreal Engine.
 */
UCLASS()
class CHEESSY_API UBpVideoSettingsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Citeste rezolutiile suportate de adaptorul video
	UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool GetSupportedScreenResolutions(TArray<FString>& Resolutions);

	// Citeste rezolutia curenta.
	UFUNCTION(BlueprintPure, Category = "Video Settings")
		static FString GetScreenResolution();

	// Verifica daca aplicatia este in fullscreen
	UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool IsInFullscreen();

	// Seteaza rezolutia dorita (Dar nu o schimba)
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

	// Schimba in rezolutia dorita
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen);

	// Citeste setarile de calitate video
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool GetVideoQualitySettings(int32& AntiAliasing, int32& Effects, int32& PostProcess, int32& Resolution, int32& Shadow, int32& Texture, int32& ViewDistance);

	// Seteaza setarile de calitate video
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetVideoQualitySettings(const int32 AntiAliasing = 3, const int32 Effects = 3, const int32 PostProcess = 3,
			const int32 Resolution = 100, const int32 Shadow = 3, const int32 Texture = 3, const int32 ViewDistance = 3);

	// Verifica daca aplicatia are VSync activat
	UFUNCTION(BlueprintPure, Category = "Video Settings")
		static bool IsVSyncEnabled();

	// Seteaza VSync
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SetVSyncEnabled(const bool VSync);

	// Confirma si seteaza rezolutia si modul fullscreen/windowed
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool SaveVideoModeAndQuality();

	// Reseteaza setarile la cele initiale
	UFUNCTION(BlueprintCallable, Category = "Video Settings")
		static bool RevertVideoMode();

private:
	// Returneaza un pointer catre obiectul responsabil setarilor aplicatie
	static UGameUserSettings* GetGameUserSettings();

};
