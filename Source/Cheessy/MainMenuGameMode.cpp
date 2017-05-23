// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "MainMenuGameMode.h"

//Initializarea variabilelor static
FString AMainMenuGameMode::displayname = "";
int AMainMenuGameMode::Elo = 0;

//Constructor pentru clasa
AMainMenuGameMode::AMainMenuGameMode()
{
	GameSparksComponent = CreateDefaultSubobject<UGameSparksComponent>(TEXT("GameSparks Component"));
}

//Functia este apelata la inceputul nivelului
void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (!UGameSparksModule::GetModulePtr()->IsInitialized())
	{
		GameSparksComponent->OnGameSparksAvailableDelegate.AddDynamic(this, &AMainMenuGameMode::OnGameSparksAvailable);
		GameSparksComponent->Disconnect();
		GameSparksComponent->Connect("GameSparks API Key", "GameSparks API Secret");
	}
}

//Functia este apelata la incheierea sesiunii
void AMainMenuGameMode::EndPlay(EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
	if (GameSparksComponent && (reason == 2 || reason == 4))
		GameSparksComponent->Disconnect();
}

//Functia trimite o cerere de autentificare catre platforma GameSparks
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

//Functie apelata dupa raspunsul la cererea de autentificare din partea platformei GameSparks
void AMainMenuGameMode::LoginRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AuthenticationResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		GameSparks::Api::Requests::AccountDetailsRequest accDetailsRequests(gs);
		accDetailsRequests.Send(LoginSuccessDetails_Response);
	}
}

//Functie apelata dupa autentificarea reusita. Creaza o cerere pentru detaliile contului autentificat.
void AMainMenuGameMode::LoginSuccessDetails_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AccountDetailsResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		GameSparks::Core::GS& gss = UGameSparksModule::GetModulePtr()->GetGSInstance();
		GameSparks::Api::Requests::AccountDetailsRequest req(gss);
		req.Send(AccountDetailsRequest_Response);
	}

}

//Functia realizeaza inregistrarea jucatorului pe platforma GameSparks si in baza de date NoSQL
void AMainMenuGameMode::Register(FString username, FString password, FString Registerdisplayname)
{
	if (username.IsEmpty() || password.IsEmpty())
		return;

	GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
	GameSparks::Api::Requests::RegistrationRequest req(gs);

	req.SetUserName(std::string(TCHAR_TO_UTF8(*username)));
	req.SetPassword(std::string(TCHAR_TO_UTF8(*password)));
	req.SetDisplayName(std::string(TCHAR_TO_UTF8(*Registerdisplayname)));

	req.Send(RegistrationRequest_Response);

	Authenticator(username, password);
}

//Functia este apelata dupa cererea de inregistrare si indica daca inregistrarea a fost reusita sau nu
void AMainMenuGameMode::RegistrationRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::RegistrationResponse & resp)
{
	/*if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, resp.GetJSONString().c_str());*/
}

//Returneaza detaliile contului autentificat. Salveaza numele si elo-ul jucatorul in client side
void AMainMenuGameMode::AccountDetailsRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::AccountDetailsResponse & resp)
{
	if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	{
		AMainMenuGameMode::displayname = UTF8_TO_TCHAR(resp.GetDisplayName().GetValue().c_str());
		AMainMenuGameMode::Elo = resp.GetCurrency1().GetValue();
	}
}

//Functia realizeaza o cerere de schimbare a numelui jucatorului pe platforma GameSparks
void AMainMenuGameMode::ChangeDisplayName(FString DisplayName)
{
	if (DisplayName.IsEmpty())
		return;

	GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
	GameSparks::Api::Requests::ChangeUserDetailsRequest req(gs);

	req.SetDisplayName(std::string(TCHAR_TO_UTF8(*DisplayName)));
	req.Send(ChangeUserDetailRequest_Response);
}

//Functia este apelata dupa cererea de schimbare a numelui jucatorului si indica daca numele a fost schimbat
void AMainMenuGameMode::ChangeUserDetailRequest_Response(GameSparks::Core::GS & gs, const GameSparks::Api::Responses::ChangeUserDetailsResponse & resp)
{
	/*if (UGameSparksModule::GetModulePtr()->IsInitialized() && !resp.GetHasErrors())
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, resp.GetJSONString().c_str());*/
}

//Functia este apelata dupa conectarea la platforma GameSpark si indica daca corectarea a fost reusita
void AMainMenuGameMode::OnGameSparksAvailable(bool available)
{
	gsavailable = available;
}

//Returneaza adresa IP locala
FString AMainMenuGameMode::LocalIP()
{
	bool AppenPort = false;
	TSharedRef<FInternetAddr> LocalAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, AppenPort);
	return LocalAddress->ToString(false);
}

//Trimite un ping la instanta de Windows Server 2012 de pe AWS Ec2
void AMainMenuGameMode::PingAWS(FString ServerPublicIP, FString ServerPort)
{
	PingResult.BindUObject(this, &AMainMenuGameMode::PingAWSResponse);
	const FString Address = ServerPublicIP + ":" + ServerPort;
	FUDPPing::UDPEcho(Address, 5.f, PingResult);
}

//Raspunsul din partea instantei de Windows Server 2012 de pe AWS Ec2
void AMainMenuGameMode::PingAWSResponse(FIcmpEchoResult Result)
{
	PingResult.Unbind();
	PingStatus = "PingFailed";
	switch (Result.Status)
	{
	case EIcmpResponseStatus::Success:
		PingStatus = "Success";
		break;
	case EIcmpResponseStatus::Timeout:
		PingStatus = "Timeout";
		break;
	case EIcmpResponseStatus::Unreachable:
		PingStatus = "Unreachable";
		break;
	case EIcmpResponseStatus::Unresolvable:
		PingStatus = "Unresolvable";
		break;
	case EIcmpResponseStatus::InternalError:
		PingStatus = "Internal Error";
		break;
	default:
		PingStatus = "Unknown Error";
		break;
	}
}
