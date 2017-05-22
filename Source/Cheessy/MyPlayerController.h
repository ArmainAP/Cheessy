// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * Aceasta este clasa pentru controlorul jucatorului. 
 */
UCLASS()
class CHEESSY_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//Constructor pentru clasa
	AMyPlayerController();

	//Indica ID-ul jucatorului
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Turn Based")
	int PlayerID;

	//Indica tura jucatorului
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Turn Based")
	int Turn;

	//Indica daca jocul ar trebui sa fie incheiat fortarea conditiei de castig
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool ForceWin;
};
