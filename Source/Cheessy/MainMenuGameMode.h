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

	//Componentul care mentine conexiunea cu serviciile GameSparks
	UPROPERTY(VisibleAnywhere)
		UGameSparksComponent* GameSparksComponent;

	//Indica daca serviciile GameSparks au fost initializate
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool gsavailable;

	//Username-ul utilizatorului
	static FString displayname;
	//Rank-ul utilizatorului
	static int Elo;

	//Indica starea ping-ului catre instanta de Windows Server 2012 de pe AWS Ec2
	FIcmpEchoResultDelegate PingResult;

	//Trimite un ping la instanta de Windows Server 2012 de pe AWS Ec2
	UFUNCTION(BlueprintCallable)
		void PingAWS(FString ServerPublicIP, FString ServerPort);

	//Raspunsul din partea instantei de Windows Server 2012 de pe AWS Ec2
	void PingAWSResponse(FIcmpEchoResult Result);

	//Afiseaza un mesaj potrivit dupa raspunul indicat de PingResult
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString PingStatus = "Ping";
};
