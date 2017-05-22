// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PiecesParent.generated.h"

/**
* Aceasta este clasa pentru piese.
*/
UCLASS()
class CHEESSY_API APiecesParent : public AActor
{
	GENERATED_BODY()
	
public:	
	//Constructor pentru clasa
	APiecesParent();

	//Indica daca piesa este alba sau neagra
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	bool Side;

	//Indica ID-ul piesei
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	int PieceID; //Rege 0, Regina 1, Nebun 2, Cal 3, Tura 4, Pion 5

	//Indica daca piesa a fost imbunatatita
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	bool Upgraded;

	//Indica numarul de unitati pe care piesa ii poate parcurge
	UPROPERTY(BlueprintReadWrite)
	int Speed;
	
	//Indica destinatia piesei in coordonate xyz 
	UPROPERTY()
	FVector DistanceToTravel;

	//Indica numarul de unitati ramase pana la destinatie
	UPROPERTY()
	int LastDistance;

	//Indica daca piesa poate fi mutata
	UPROPERTY(Replicated)
	bool CanMovePiece;

	//Declararea componentului pentru coliziune
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComponent;

	//Declararea componentului pentru modelul 3D al piesei
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Destructible;

	//Declararea componentului pentru rotatia piesei
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RotationComponent;

	//Declararea componentului pentru efectulu vizual al Nebunului si a Turei dupa imbunatatire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* Wave;

	//Declararea componentului pentru efectul vizual al Nebunului si a Turei dupa imbunatatire
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* WaveCollision;

	//Indica daca piesa are scut
	bool Shielded = false;

	//Indica daca piesa are imunitate
	bool Immunity = false;

	//Indica daca ar trebui afisat efectul de telportare a reginei
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DistortionParticle;

	//Indica daca ar trebui afisat efect de teleportare a testinatie a reginei
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TeleportParticle;

	//Indica daca piesa a fost imbunatatita odata
	bool UpgradeOnce = true;

	//Indica daca piesa poate fi imbunatatita de mai multe ori
	bool ChangeOnce;

	//Indica daca piesa ar trebui sa isi reimprospateze textura astfel incat textura sa afiseze la margini o anumita culoare
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RefreshOutline;

	//Indica culoarea pe care textura trebuie sa o afiseze la margini
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor OutlineColor;

protected:
	//Apelata la inceperea jocului
	virtual void BeginPlay() override;

public:	
	//Functia este apelata la fiecare frame
	virtual void Tick(float DeltaTime) override;
	//Returneaza daunele primite
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//Replica daunele primite la fiecare client
	UFUNCTION(NetMulticast, Reliable)
		void BroadcastDamagePiece(APiecesParent* DamagingPiece);

	//Functia elimina scutul pieselor
	void UnShielded();

	//Functia genereaza o pozitie noua neocupata de alta piesa
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerTeleport();

	//Functia muta regina pe noua pozitie generata
	UFUNCTION(NetMulticast, Reliable)
		void Teleport(const float x, const float y, const float z);

	//Functia detecteaza coliziunea abilitatii Nebunului si Turei cu alte piese
	UFUNCTION()
	void WaveBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Functia detecteaza iesirea din coliziune cu o alta piesa a abilitatii Nebunului si Turei
	UFUNCTION()
	void WaveEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
