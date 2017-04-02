// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class CHEESSY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

public:
	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY()
		bool LeftMousePressed;

	UPROPERTY()
		APiecesParent* SelectedActor = nullptr;

	UPROPERTY()
		APiecesParent* ActorToMove = nullptr;

	UPROPERTY()
		APiecesParent* DisableCollision = nullptr;

	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void Forward(float Value);
	void Strafe(float Value);
	void Up(float Value);
	void Left(float Value);
	void RightClickDown();
	void RightClickUp();
	void LeftClickDown();
	void LeftClickUp();

	FRotator PieceRotation();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRotatePiece(const FRotator& rot, APiecesParent* Piece);

	UFUNCTION(NetMulticast, Reliable)
		void BroadcastRotation(const FRotator rot, APiecesParent* Piece);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeTurn(const int& LastTurn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMovePiece(const FVector& Speed, const int& LastDistance, APiecesParent* Piece);

	UFUNCTION(NetMulticast, Reliable)
		void MovePiece(const FVector& Speed, const int& LastDistance, APiecesParent* Piece);

	UFUNCTION(NetMulticast, Reliable)
		void StopMovePiece(APiecesParent* Piece);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPieceDamage(APiecesParent* PieceToDamage, APlayerController* PlayerController, APiecesParent* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
		void BroadcastUpgradePiece(APiecesParent* Piece);

	void EnableCollision();
};