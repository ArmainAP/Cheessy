// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHEESSY_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Turn Based")
	int PlayerID;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Turn Based")
	int Turn;
};
