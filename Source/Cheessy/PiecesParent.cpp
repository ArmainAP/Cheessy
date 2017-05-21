// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "UnrealNetwork.h"
#include "PiecesParent.h"
#include "OnlineGameMode.h"
#include "MyCharacter.h"

//Constructor pentru APiecesParent
APiecesParent::APiecesParent()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetIsReplicated(true);

	RotationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RotationComponent"));
	RotationComponent->SetupAttachment(CapsuleComponent);

	Destructible = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Destructible->SetIsReplicated(true);
	Destructible->SetWorldScale3D(FVector(10.0f, 10.0f, 10.0f));
	Destructible->SetupAttachment(CapsuleComponent);

	Wave = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Wave"));
	Wave->SetupAttachment(CapsuleComponent);

	WaveCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WaveCollision"));
	WaveCollision->SetSphereRadius(150.0f);
	WaveCollision->SetupAttachment(CapsuleComponent);
}

//Replica variabila care detine date despre starea de miscare a piesei
void APiecesParent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APiecesParent, CanMovePiece);
}

//Apelata la inceperea jocului
void APiecesParent::BeginPlay()
{
	Super::BeginPlay();

	WaveCollision->OnComponentBeginOverlap.AddDynamic(this, &APiecesParent::WaveBeginOverlap);
	WaveCollision->OnComponentEndOverlap.AddDynamic(this, &APiecesParent::WaveEndOverlap);

		//ID-ul pieselor: Rege = 0, Regina = 1, Nebun = 2, Cal = 3, Tura = 4, Pion = 5
		switch (PieceID)
		{
		default:
			break;

		case 0:
			Speed = 200;
			break;

		case 1:
			Speed = 700;
			break;

		case 2:
			Speed = 500;
			break;

		case 3:
			Speed = 300;
			break;

		case 4:
			Speed = 500;
			break;

		case 5:
			Speed = 100;
			break;
		}
}

//Functia este apelata la fiecare frame
void APiecesParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMovePiece)
	{
		if (DistanceToTravel.Size2D() == 0.0f)
		{
			DistanceToTravel = GetActorLocation() + FVector(round(GetActorForwardVector().X * 10) / 10, round(GetActorForwardVector().Y * 10) / 10, 0.0f) * Speed;
			LastDistance = FMath::FloorToInt((GetActorLocation() - DistanceToTravel).Size2D());
			ChangeOnce = false;
		}
		AMyCharacter* Pawn = Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if(Pawn)
			Pawn->ServerMovePiece(GetActorLocation() + FVector(round(GetActorForwardVector().X * 10) / 10, round(GetActorForwardVector().Y * 10) / 10, 0.0f) * 500 * DeltaTime, LastDistance, this);
	}
	else if (ChangeOnce)
	{
		Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->ServerChangeTurn(2);
		DistanceToTravel = FVector(0.0f, 0.0f, 0.0f);
		ChangeOnce = false;
	}

	if (Upgraded && UpgradeOnce)
	{
		switch (PieceID)
		{
		default:
			break;

		case 0:
			Shielded = true;
			Upgraded = false;
			break;

		case 1:
			CanMovePiece = false;
			ServerTeleport();
			Upgraded = false;
			break;

		case 2:
			Wave->SetHiddenInGame(false);
			Immunity = false;
			Shielded = false;
			UpgradeOnce = false;
			break;

		case 3:
			UpgradeOnce = false;
			break;

		case 4:
			Wave->SetHiddenInGame(false);
			Immunity = false;
			Shielded = false;
			UpgradeOnce = false;
			break;

		case 5:
			Speed = Speed * 2;
			Upgraded = false;
			break;
		}
	}

	if (!UpgradeOnce)
	{
		OutlineColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
		RefreshOutline = true;
	}
	else if (Immunity)
	{
		OutlineColor = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
		RefreshOutline = true;
		Shielded = true;
	}
	else if (Shielded)
	{
		OutlineColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f);
		RefreshOutline = true;
	}
	else
	{
		OutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		RefreshOutline = true;
	}
}

//Returneaza daunele primite
float APiecesParent::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	BroadcastDamagePiece(Cast<APiecesParent>(DamageCauser));
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

//Replica daunele primite la fiecare client
void APiecesParent::BroadcastDamagePiece_Implementation(APiecesParent* DamagingPiece)
{
	if (Shielded)
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &APiecesParent::UnShielded, GetWorld()->GetDeltaSeconds(), false);
	}
	else
	{
		if (DamagingPiece->PieceID == 1)
			DistortionParticle = true;
		SetLifeSpan(0.1f);
	}
}

//Functia elimina scutul pieselor
void APiecesParent::UnShielded()
{
	Shielded = false;
}

//Functia valideaza APiecesParent::ServerTeleport_Implementation pe server
bool APiecesParent::ServerTeleport_Validate()
{
	return true;
}

//Functia genereaza o pozitie noua neocupata de alta piesa
void APiecesParent::ServerTeleport_Implementation()
{
	FHitResult TeleportHit;
	float x, y, z;
	x = FMath::RandRange(-350, 350);
	y = FMath::RandRange(-350, 350);
	z = GetActorLocation().Z;
	if (SetActorLocation(FVector(x, y, z), true, &TeleportHit) == false)
		ServerTeleport();
	else
		Teleport(x, y, z);
}

//Functia muta regina pe noua pozitie generata
void APiecesParent::Teleport_Implementation(const float x, const float y, const float z)
{
	SetActorLocation(FVector(x, y, z));
	LastDistance = 0.0f;
	CanMovePiece = true;
	TeleportParticle = true;
}

//Functia detecteaza coliziunea abilitatii Nebunului si Turei cu alte piese
void APiecesParent::WaveBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APiecesParent* OverlappedPiece = Cast<APiecesParent>(OtherActor);
	if (OverlappedPiece && OverlappedPiece->CapsuleComponent == OtherComp)
	{
		if (UpgradeOnce == false && (OverlappedPiece->UpgradeOnce && OverlappedPiece->Side == Side) && PieceID == 4)
			OverlappedPiece->Immunity = true;

		if (UpgradeOnce == false && PieceID == 2 && OverlappedPiece->Side != Side)
		{
			OverlappedPiece->Immunity = false;
			OverlappedPiece->Shielded = false;
			if (OverlappedPiece->PieceID == 1)
				OverlappedPiece->Upgraded = true;
		}
	}
}

//Functia detecteaza iesirea din coliziune cu o alta piesa a abilitatii Nebunului si Turei
void APiecesParent::WaveEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiecesParent* OverlappedPiece = Cast<APiecesParent>(OtherActor);
	if (OverlappedPiece && PieceID == 4)
		OverlappedPiece->Immunity = false;
}