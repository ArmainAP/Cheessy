// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "LoginUserWidget.generated.h"

/**
 * Aceasta clasa este pentru interfata meniului principal
 */
UCLASS()
class CHEESSY_API ULoginUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//Email-ul introdus in text box
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString username;

	//Parola introdusa in text box
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString password;

	//Numele introdus in text box
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		FString displayname;

	//Rank-ul jucatorului
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Login Screen")
		int Elo;

	//Apeleaza functia de autentificare din AMainMenuGameMode
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void LoginClicked();

	//Apeleaza functia de inregistrare din AMainMenuGameMode
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void RegisterClicked();

	//Salveaza valorile variabilelor displayname si Elo din Cloud pe client size. Modificarile asupra acestor valori in client nu modifica valoarea reala din Cloud!
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void OnlineClicked();

	//Apeleaza functia de schimbare a numelui din AMainMenuGameMode
	UFUNCTION(BlueprintCallable, Category = "Login Screen")
		void ChangeDisplayname();
};
