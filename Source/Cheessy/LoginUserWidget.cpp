// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "LoginUserWidget.h"
#include "MainMenuGameMode.h"

void ULoginUserWidget::LoginClicked()
{
	AMainMenuGameMode* GM = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GameSparksComponent->IsAvailable())
		GM->Authenticator(username, password);
}

void ULoginUserWidget::RegisterClicked()
{
	AMainMenuGameMode* GM = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && GM->GameSparksComponent->IsAvailable())
		GM->Register(username, password, displayname);
}

void ULoginUserWidget::OnlineClicked()
{
	displayname = AMainMenuGameMode::displayname;
	Elo = AMainMenuGameMode::Elo;
}
