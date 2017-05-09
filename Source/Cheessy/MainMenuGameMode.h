// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Http.h"
#include "GameFramework/GameModeBase.h"
#include "GameSparks/Private/GameSparksComponent.h"
#include "GameSparks/Private/GSApi.h"
#include "GameSparks/Public/GameSparksModule.h"
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSResponses.h>
#include <GameSparks/generated/GSRequests.h>
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHEESSY_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type reason);

	void Authenticator(FString username, FString password);
	static void LoginRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AuthenticationResponse &resp);
	static void LoginSuccessDetails_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AccountDetailsResponse &resp);

	void Register(FString username, FString password, FString displayname);
	static void RegistrationRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::RegistrationResponse &resp);

	static void AccountDetailsRequest_Response(GameSparks::Core::GS &gs, const GameSparks::Api::Responses::AccountDetailsResponse &resp);

	UFUNCTION()
		void OnGameSparksAvailable(bool available);

	UFUNCTION(BlueprintPure, Category = "IP")
		FString LocalIP();

	UFUNCTION(BlueprintCallable, Category = "IP")
		bool ExternalIP_SentRequest();

	UFUNCTION(BlueprintImplementableEvent, Category = "IP", meta = (DisplayName = "ExternalIP ~ Data Received!"))
		void ExternalIP_DataReceived(const FString& YourIP);

	void HTTPOnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY(VisibleAnywhere)
		UGameSparksComponent* GameSparksComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool gsavailable;

	static FString displayname;
	static int Elo;
};
