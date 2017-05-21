// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"

//Constructor pentru AMyPlayerController
AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

//Replica variabilele ce detin date deste ID-ul si randul jucatorului.
void AMyPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerController, PlayerID);
	DOREPLIFETIME(AMyPlayerController, Turn);
}