// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "OnlineGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHEESSY_API AOnlineGameMode : public AGameMode
{
	GENERATED_BODY()
	
private:
	virtual void PostLogin(APlayerController * NewPlayer) override;
	virtual void Logout(AController * ExitPlayer) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<APlayerController*> PlayerArray;

	UPROPERTY(Replicated)
	int Turn;

	UPROPERTY()
	int LastPlayerTurn;

	UPROPERTY(Replicated)
	int PlayerID;

	UPROPERTY(Replicated)
		bool MultiPlayer;

	int DCount;

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastTurn(const int& LastTurn); //0 player 1, 1 player 2, 2 server
};