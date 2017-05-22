// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

/**
* Aceasta este clasa pentru jucator.
*/
UCLASS()
class CHEESSY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

		//Referinta pentru componentul responsabil de rendering
		UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

public:
	//Constructor pentru clasa
	AMyCharacter();

	//Indica daca butonul click dreapta mouse a fost apasata de 2 ori
	bool RightDoubleClickPressed;
	//Indica daca butonul click stanga mouse este apasat
	bool LeftMousePressed;
	//Indica cat timp a fost tasta apasata
	float KeyDownTime;

	//Indica o referinta fata de piesa selectata
	UPROPERTY()
		APiecesParent* SelectedActor = nullptr;

	//Indica o referinta fata de o piesa fara coliziune
	UPROPERTY()
		APiecesParent* DisableCollision = nullptr;

	//Componentul pentru rendering
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

public:
	//Functia este apelata la fiecare frame
	virtual void Tick(float DeltaTime) override;

	//Functie apelata la apasarea tastei 'W' sau 'S'
	void Forward(float Value);
	//Functie apelata la apasarea tastei 'A' sau 'D'
	void Strafe(float Value);
	//Functie apelata la schimbarea coordonatei X a mouse-ului
	void Up(float Value);
	//Functie apelata la schimbarea coordonatei Y a mouse-ului
	void Left(float Value);
	//Functie apelata dupa oprirea apasarii click dreapta mouse
	void RightClickDown();
	//Functie apelata dupa apasarea click dreapta mouse
	void RightClickUp();
	//Functie apelata dupa apasarea dubla a click dreapta mouse
	void RightDoubleClick();
	//Functie apelata dupa apasarea click stanga mouse
	void LeftClickDown();
	//Functie apelata dupa oprirea apasarii click stanga mouse
	void LeftClickUp();
	//Functie apelata dupa apasarea dubla a click stanga mouse
	void LeftDoubleClick();

	//Returneaza rotatia indreptata spre pozitia mouse-ului proiectata in spatiu 3Dimensional
	FRotator PieceRotation();

	//Functia seteaza tastele
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Functia transmite rotatia catre fiecare client
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRotatePiece(const FRotator& rot, APiecesParent* Piece);

	//Aplica rotatia primita de la server
	UFUNCTION(NetMulticast, Reliable)
		void BroadcastRotation(const FRotator rot, APiecesParent* Piece);

	//Functia transmite schimbarea randului intre jucatori
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeTurn(const int& LastTurn);

	//Functia transmite deplasarea piesei catre client
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMovePiece(const FVector& Speed, const int& LastDistance, APiecesParent* Piece);

	//Functia replica deplasarea pieselor pe fiecare client
	UFUNCTION(NetMulticast, Reliable)
		void MovePiece(const FVector& Speed, const int& LastDistance, APiecesParent* Piece);

	//Functia este apelata la oprirea deplasarii pieselor
	UFUNCTION(NetMulticast, Reliable)
		void StopMovePiece(APiecesParent* Piece);

	//Functia transmite daunele pieselor in cazul coliziunii pieselor de culori diferite care nu au scut catre client
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPieceDamage(APiecesParent* PieceToDamage, APlayerController* PlayerController, APiecesParent* DamageCauser);

	//Functia replica imbunatarirea pieselor catre client
	UFUNCTION(NetMulticast, Reliable)
		void BroadcastUpgradePiece(APiecesParent* Piece);

	//Functia reactiveaza coliziunea
	void EnableCollision();
};