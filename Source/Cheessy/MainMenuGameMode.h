// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Http.h"
#include "Icmp.h"
#include "GameFramework/GameModeBase.h"
#include "GameSparks/Private/GameSparksComponent.h"
#include "GameSparks/Private/GSApi.h"
#include "GameSparks/Public/GameSparksModule.h"
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSResponses.h>
#include <GameSparks/generated/GSRequests.h>
#include "MainMenuGameMode.generated.h"

/**
 * Aceasta este clasa de baza pentru meniul principal.
 */
UCLASS()
class CHEESSY_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//Constructor pentru clasa
	AMainMenuGameMode();

	//Functia este apelata la inceputul nivelului
	virtual void BeginPlay() override;
	//Functia este apelata la incheierea sesiunii
	virtual void EndPlay(EEndPlayReason::Type reason);

	//Functia trimite o cerere de autentificare catre platforma GameSparks
	void Authenticator(FString username, FString password);
	//Functie apelata dupa raspunsul la cererea de autentificare din partea platformei GameSparks
	static void LoginRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AuthenticationResponse &resp);
	//Functie apelata dupa autentificarea reusita. Creaza o cerere pentru detaliile contului autentificat.
	static void LoginSuccessDetails_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AccountDetailsResponse &resp);

	//Functia realizeaza inregistrarea jucatorului pe platforma GameSparks si in baza de date NoSQL
	void Register(FString username, FString password, FString Registerdisplayname);
	//Functia este apelata dupa cererea de inregistrare si indica daca inregistrarea a fost reusita sau nu
	static void RegistrationRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::RegistrationResponse &resp);
	
	//Returneaza detaliile contului autentificat. Salveaza numele si elo-ul jucatorul in client side
	static void AccountDetailsRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AccountDetailsResponse &resp);

	void ChangeDisplayName(FString DisplayName);
	static void ChangeUserDetailRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::ChangeUserDetailsResponse &resp);

	//Functia este apelata dupa conectarea la platforma GameSpark si indica daca corectarea a fost reusita
	UFUNCTION()
		void OnGameSparksAvailable(bool available);

	//Returneaza adresa IP locala
	UFUNCTION(BlueprintPure, Category = "IP")
		FString LocalIP();

	//Realizeaza o cerere HTTP catre http://api.ipify.org/
	UFUNCTION(BlueprintCallable, Category = "IP")
		bool ExternalIP_SentRequest();

	//Aceasta functie returneaza IP-ul external si poate realiza apelul altor functii in Visual Scripting
	UFUNCTION(BlueprintImplementableEvent, Category = "IP", meta = (DisplayName = "ExternalIP ~ Data Received!"))
		void ExternalIP_DataReceived(const FString& YourIP);

	//Returneaza adresa IP externala/publica indicata de http://api.ipify.org/
	void HTTPOnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(VisibleAnywhere)
		UGameSparksComponent* GameSparksComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool gsavailable;

	static FString displayname;
	static int Elo;

	FIcmpEchoResultDelegate PingResult;

	UFUNCTION(BlueprintCallable)
		void CheckIfServerOnline(FString ServerPublicIP, FString ServerPort);

	void OnServerCheckFinished(FIcmpEchoResult Result);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString PingStatus = "Ping";
};
