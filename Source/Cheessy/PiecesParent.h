// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PiecesParent.generated.h"

UCLASS()
class CHEESSY_API APiecesParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiecesParent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	bool Side;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	int PieceID; //King 0, Queen 1, Bishop 2, Horse 3, Rook 4, Pawn 5

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	bool Upgraded;

	UPROPERTY(BlueprintReadWrite)
	int Speed;
	
	UPROPERTY()
	FVector DistanceToTravel;

	UPROPERTY()
	int LastDistance;

	UPROPERTY(Replicated)
	bool CanMovePiece;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Destructible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RotationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* Wave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* WaveCollision;

	bool Shielded = false;

	bool Immunity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DistortionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TeleportParticle;

	bool UpgradeOnce = true;

	bool ChangeOnce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RefreshOutline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor OutlineColor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(NetMulticast, Reliable)
		void BroadcastDamagePiece(APiecesParent* DamagingPiece);

	void UnShielded();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerTeleport();

	UFUNCTION(NetMulticast, Reliable)
		void Teleport(const float x, const float y, const float z);

	UFUNCTION()
	void WaveBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void WaveEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
