// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "LoginUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHEESSY_API ULoginUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString username;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString password;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString displayname;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		int Elo;

	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void LoginClicked();
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void RegisterClicked();
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void OnlineClicked();
};
