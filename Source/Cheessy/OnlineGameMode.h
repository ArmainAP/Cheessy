// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "OnlineGameMode.generated.h"

/**
 * Aceasta este clasa de baza pentru joc.
 */
UCLASS()
class CHEESSY_API AOnlineGameMode : public AGameMode
{
	GENERATED_BODY()
	
private:
	//Functia este apelata dupa conectarea fiecarui jucator
	virtual void PostLogin(APlayerController * NewPlayer) override;
	//Functia este apelata dupa deconectarea fiecarui jucator
	virtual void Logout(AController * ExitPlayer) override;

public:
	//Referinta pentru jucatorii conectati
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<APlayerController*> PlayerArray;

	//Tura jucatorului
	UPROPERTY(Replicated)
	int Turn; //0 jucator, 1 jucator, 2 server

	//Tura ce a trecut
	UPROPERTY()
	int LastPlayerTurn;

	//ID-ul server-ului ca jucator
	UPROPERTY(Replicated)
	int PlayerID;

	//Indica daca jocul este "Hotseat" sau nu (Hotseat este un termen folosit pentru un joc pe ture in care jucatorii se joaca pe acelasi calculator)
	UPROPERTY(Replicated)
		bool MultiPlayer;

	//Indica cati jucatori s-au deconectat de pe server
	int DCount;

	//Aplica schimbarea randului pe fiecare client
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastTurn(const int& LastTurn); //0 jucator, 1 jucator, 2 server
};