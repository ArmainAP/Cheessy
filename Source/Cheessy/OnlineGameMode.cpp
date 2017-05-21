// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "MyPlayerController.h"
#include "PiecesParent.h"
#include "UnrealNetwork.h"
#include "OnlineGameMode.h"
#include "MyCharacter.h"

//Replica variabilele ce detin date deste ID-ul, randul jucatorului si daca jocul este local sau prin internet
void AOnlineGameMode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOnlineGameMode, Turn);
	DOREPLIFETIME(AOnlineGameMode, PlayerID);
	DOREPLIFETIME(AOnlineGameMode, MultiPlayer);
}

//Functia este apelata dupa conectarea fiecarui jucator
void AOnlineGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer);
	PlayerArray.Add(NewPlayer);
	PlayerID = PlayerArray.Num() - 1;
	PC->PlayerID = PlayerID;
	if (PlayerID == 1)
		MultiPlayer = true;
}

void AOnlineGameMode::Logout(AController * ExitPlayer)
{
	Super::Logout(ExitPlayer);

	DCount++;
	if (DCount > 1)
		FGenericPlatformMisc::RequestExit(false);

	for (int i = 0; i < PlayerArray.Num(); i++)
		if (PlayerArray[i] != ExitPlayer->CastToPlayerController())
		{
			AMyPlayerController* PC = Cast<AMyPlayerController>(PlayerArray[i]);
			PC->ForceWin = true;
		}
}

//Replica schimbarea randului intre jucatori catre fiecare client
void AOnlineGameMode::BroadcastTurn_Implementation(const int& LastTurn)
{
	if (LastTurn != 2)
	{
		LastPlayerTurn = LastTurn;
		Turn = 2;
	}
	else
	{
		Turn = (LastPlayerTurn == 0) ? 1 : 0;
		if (MultiPlayer == false)
		{
			AMyPlayerController* PC = Cast<AMyPlayerController>(PlayerArray[0]);
			PC->PlayerID = Turn;
		}
	}

	for (int i = 0; i < PlayerArray.Num(); i++)
	{
		AMyPlayerController* PC = Cast<AMyPlayerController>(PlayerArray[i]);
		PC->Turn = Turn;
	}
}