// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "MainMenuGameMode.h"

FString AMainMenuGameMode::displayname = "";
int AMainMenuGameMode::Elo = 0;

AMainMenuGameMode::AMainMenuGameMode()
{
	GameSparksComponent = CreateDefaultSubobject<UGameSparksComponent>(TEXT("GameSparks Component"));
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (!UGameSparksModule::GetModulePtr()->IsInitialized())
	{
		GameSparksComponent->OnGameSparksAvailableDelegate.AddDynamic(this, &AMainMenuGameMode::OnGameSparksAvailable);
		GameSparksComponent->Disconnect();
		GameSparksComponent->Connect("C309664r41x6", "xsFXAlNATB1Z9faoVPrwoDv2yos1LyyZ");
	}
}

void AMainMenuGameMode::EndPlay(EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
	if (GameSparksComponent && (reason == 2 || reason == 4))
		GameSparksComponent->Disconnect();
}

void AMainMenuGameMode::Authenticator(FString username, FString password)
{
	if (username.IsEmpty() || password.IsEmpty())
		return;

	GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
	GameSparks::Api::Requests::AuthenticationRequest req(gs);

	req.SetUserName(std::string(TCHAR_TO_UTF8(*username)));
	req.SetPassword(std::string(TCHAR_TO_UTF8(*password)));

	req.Send(LoginRequest_Response);
}

void AMainMenuGameMode::LoginRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AuthenticationResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		GameSparks::Api::Requests::AccountDetailsRequest accDetailsRequests(gs);
		accDetailsRequests.Send(LoginSuccessDetails_Response);
	}
}

void AMainMenuGameMode::LoginSuccessDetails_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AccountDetailsResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
		GameSparks::Api::Requests::AccountDetailsRequest req(gs);
		req.Send(AccountDetailsRequest_Response);
	}

}

void AMainMenuGameMode::Register(FString username, FString password, FString displayname)
{
	if (username.IsEmpty() || password.IsEmpty())
		return;

	GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
	GameSparks::Api::Requests::RegistrationRequest req(gs);

	req.SetUserName(std::string(TCHAR_TO_UTF8(*username)));
	req.SetPassword(std::string(TCHAR_TO_UTF8(*password)));
	req.SetDisplayName(std::string(TCHAR_TO_UTF8(*displayname)));

	req.Send(RegistrationRequest_Response);

	Authenticator(username, password);
}

void AMainMenuGameMode::RegistrationRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::RegistrationResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, resp.GetJSONString().c_str());
}

void AMainMenuGameMode::AccountDetailsRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AccountDetailsResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		AMainMenuGameMode::displayname = UTF8_TO_TCHAR(resp.GetDisplayName().GetValue().c_str());
		AMainMenuGameMode::Elo = resp.GetCurrency1().GetValue();
	}
}

void AMainMenuGameMode::OnGameSparksAvailable(bool available)
{
	gsavailable = available;
	if (available)
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Available"));
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Not Available"));
}

FString AMainMenuGameMode::LocalIP()
{
	bool AppenPort = false;
	TSharedRef<FInternetAddr> LocalAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, AppenPort);
	return LocalAddress->ToString(false);
}

bool AMainMenuGameMode::ExternalIP_SentRequest()
{
	FHttpModule* Http = &FHttpModule::Get();

	if(!Http || !Http->IsHttpEnabled())
		return false;

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(FString("http://api.ipify.org/"));
	Request->SetHeader("User-Agent", "Cheessy");
	Request->SetHeader("Content-Type", "text/html");

	Request->OnProcessRequestComplete().BindUObject(this, &AMainMenuGameMode::HTTPOnResponseReceived);
	if (!Request->ProcessRequest())
		return false;
	return true;
}

void AMainMenuGameMode::HTTPOnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
		this->ExternalIP_DataReceived(Response->GetContentAsString());
	else
		this->ExternalIP_DataReceived(FString("HTTPOnResponseReceived >>> Connection Error"));
}
