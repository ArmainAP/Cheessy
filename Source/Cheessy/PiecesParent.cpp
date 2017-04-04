// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "UnrealNetwork.h"
#include "PiecesParent.h"
#include "OnlineGameMode.h"
#include "MyCharacter.h"

// Sets default values
APiecesParent::APiecesParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;

	//Components
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetIsReplicated(true);

	RotationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RotationComponent"));
	RotationComponent->SetupAttachment(CapsuleComponent);

	Destructible = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Mesh"));
	Destructible->SetIsReplicated(true);
	Destructible->SetWorldScale3D(FVector(10.0f, 10.0f, 10.0f));
	Destructible->SetupAttachment(CapsuleComponent);

	Wave = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Wave"));
	Wave->SetupAttachment(CapsuleComponent);

	WaveCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WaveCollision"));
	WaveCollision->SetSphereRadius(150.0f);
	WaveCollision->SetupAttachment(CapsuleComponent);
}

void APiecesParent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(APiecesParent, CanMovePiece);
}

// Called when the game starts or when spawned
void APiecesParent::BeginPlay()
{
	Super::BeginPlay();

	WaveCollision->OnComponentBeginOverlap.AddDynamic(this, &APiecesParent::WaveBeginOverlap);
	WaveCollision->OnComponentEndOverlap.AddDynamic(this, &APiecesParent::WaveEndOverlap);

		//King 0, Queen 1, Bishop 2, Horse 3, Rook 4, Pawn 5
		switch (PieceID)
		{
		default:
			break;

		case 0:
			Speed = 48;
			break;

		case 1:
			Speed = 168;
			break;

		case 2:
			Speed = 120;
			break;

		case 3:
			Speed = 70;
			break;

		case 4:
			Speed = 120;
			break;

		case 5:
			Speed = 25;
			break;
		}
}

// Called every frame
void APiecesParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMovePiece)
	{
		if (DistanceToTravel.Size2D() == 0.0f)
		{
			DistanceToTravel = GetActorLocation() + FVector(round(floor(GetActorForwardVector().X * 10) / 10), round(floor(GetActorForwardVector().Y * 10) / 10), 0.0f) * Speed;
			LastDistance = FMath::FloorToInt((GetActorLocation() - DistanceToTravel).Size2D());
			ChangeOnce = false;
		}
		AMyCharacter* Pawn = Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if(Pawn)
			Pawn->ServerMovePiece((GetActorLocation() + (GetActorForwardVector() * (GetWorld()->GetDeltaSeconds() * 500))), LastDistance, this);
	}
	else if (ChangeOnce)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("1337")));
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
		Destructible->SetCustomDepthStencilValue(3);
	else if (Immunity)
	{
		Destructible->SetCustomDepthStencilValue(2);
		Shielded = true;
	}
	else if (Shielded)
		Destructible->SetCustomDepthStencilValue(1);
	else
		Destructible->SetCustomDepthStencilValue(0);
}

float APiecesParent::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	BroadcastDamagePiece(Cast<APiecesParent>(DamageCauser));
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void APiecesParent::BroadcastDamagePiece_Implementation(APiecesParent* DamagingPiece)
{
	if (Shielded)
	{
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &APiecesParent::UnShielded, 0.1f, false);
	}
	else
	{
		if (DamagingPiece->PieceID == 1)
			DistortionParticle = true;
		Destructible->ApplyDamage(10000.0f, GetActorLocation(), DamagingPiece->GetActorForwardVector(), 100.0f);
		SetLifeSpan(1.0f);
	}
}

void APiecesParent::UnShielded()
{
	Shielded = false;
}

//Queen
bool APiecesParent::ServerTeleport_Validate()
{
	return true;
}

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

void APiecesParent::Teleport_Implementation(const float x, const float y, const float z)
{
	SetActorLocation(FVector(x, y, z));
	LastDistance = 0.0f;
	CanMovePiece = true;
	TeleportParticle = true;
}

//Rook and Bishop
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

void APiecesParent::WaveEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APiecesParent* OverlappedPiece = Cast<APiecesParent>(OtherActor);
	if (OverlappedPiece && PieceID == 4)
		OverlappedPiece->Immunity = false;
}