// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "LoginUserWidget.h"
#include "MainMenuGameMode.h"

//Apeleaza functia de autentificare din AMainMenuGameMode
void ULoginUserWidget::LoginClicked()
{
	AMainMenuGameMode* GM = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GameSparksComponent->IsAvailable())
		GM->Authenticator(username, password);
}

//Apeleaza functia de inregistrare din AMainMenuGameMode
void ULoginUserWidget::RegisterClicked()
{
	AMainMenuGameMode* GM = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GameSparksComponent->IsAvailable())
		GM->Register(username, password, displayname);
}

//Salveaza valorile variabilelor displayname si Elo din Cloud pe client size. Modificarile asupra acestor valori in client nu modifica valoarea reala din Cloud!
void ULoginUserWidget::OnlineClicked()
{
	displayname = AMainMenuGameMode::displayname;
	Elo = AMainMenuGameMode::Elo;
}

void ULoginUserWidget::ChangeDisplayname()
{
	AMainMenuGameMode* GM = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GameSparksComponent->IsAvailable())
		GM->ChangeDisplayName(displayname);
}
