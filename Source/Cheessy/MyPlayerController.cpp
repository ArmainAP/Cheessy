// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AMyPlayerController, PlayerID);
	DOREPLIFETIME(AMyPlayerController, Turn);
}