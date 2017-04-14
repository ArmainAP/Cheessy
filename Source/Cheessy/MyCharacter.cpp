// Fill out your copyright notice in the Description page of Project Settings.

#include "Cheessy.h"
#include "MyPlayerController.h"
#include "PiecesParent.h"
#include "UnrealNetwork.h"
#include "MyCharacter.h"
#include "OnlineGameMode.h"

// Sets default values
AMyCharacter::AMyCharacter()
{	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (RightDoubleClickPressed && Controller != NULL)
			AddMovementInput(Controller->GetActorForwardVector(), 1.0f, false);

	if (LeftMousePressed && SelectedActor)
	{
		KeyDownTime += DeltaTime;
		if(KeyDownTime > 0.1f)
			ServerRotatePiece(PieceRotation(), SelectedActor);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Forward", this, &AMyCharacter::Forward);
	InputComponent->BindAxis("Strafe", this, &AMyCharacter::Strafe);

	InputComponent->BindAxis("Up", this, &AMyCharacter::Up);
	InputComponent->BindAxis("Left", this, &AMyCharacter::Left);

	InputComponent->BindAction("MoveCamera", IE_Pressed, this, &AMyCharacter::RightClickDown);
	InputComponent->BindAction("MoveCamera", IE_Released, this, &AMyCharacter::RightClickUp);
	InputComponent->BindAction("MoveCamera", IE_DoubleClick, this, &AMyCharacter::RightDoubleClick);

	InputComponent->BindAction("Select", IE_Pressed, this, &AMyCharacter::LeftClickDown);
	InputComponent->BindAction("Select", IE_Released, this, &AMyCharacter::LeftClickUp);
	InputComponent->BindAction("Select", IE_DoubleClick, this, &AMyCharacter::LeftDoubleClick);
}

void AMyCharacter::Forward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(Controller->GetActorForwardVector(), Value, false);
	}
}

void AMyCharacter::Strafe(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(Controller->GetActorRightVector(), Value, false);
	}
} 

void AMyCharacter::Up(float Value)
{
	if (PLATFORM_WINDOWS || PLATFORM_LINUX || PLATFORM_MAC)
	{
		if ((Controller != NULL) && (Value != 0.0f) && (GetWorld()->GetFirstPlayerController()->bShowMouseCursor == false))
		{
			AddControllerPitchInput(Value);
		}
	}
	else
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			AddControllerPitchInput(Value * 45.0f * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AMyCharacter::Left(float Value)
{
	if (PLATFORM_WINDOWS || PLATFORM_LINUX || PLATFORM_MAC)
	{
		if ((Controller != NULL) && (Value != 0.0f) && (GetWorld()->GetFirstPlayerController()->bShowMouseCursor == false))
		{
			AddControllerYawInput(Value);
		}
	}
	else
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			AddControllerYawInput(Value * 45.0f * GetWorld()->GetDeltaSeconds());
		}
	}
}

void AMyCharacter::RightClickUp()
{
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	RightDoubleClickPressed = false;
}

void AMyCharacter::RightClickDown()
{
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
}

void AMyCharacter::RightDoubleClick()
{
	/*AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	FVector WorldLocation, WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult Hit;
	SetActorLocation(WorldLocation + GetActorForwardVector() * 1000, true, &Hit);
	if (Hit.bBlockingHit)
		SetActorLocation(Hit.ImpactPoint - GetActorForwardVector() * 100);
	AddControllerYawInput(90.0f);*/
	RightDoubleClickPressed = true;
}

void AMyCharacter::LeftClickDown()
{
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	if (PC->PlayerID == PC->Turn)
	{
		FHitResult Hit;
		TArray<TEnumAsByte<EObjectTypeQuery>> oTypes;
		oTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		PC->GetHitResultUnderCursorForObjects(oTypes, false, Hit);
		APiecesParent* HitPiece = Cast<APiecesParent>(Hit.GetActor());
		if (HitPiece)
		{
			if (((PC->PlayerID == 0) ? false : true) == HitPiece->Side)
			{
				SelectedActor = HitPiece;
				KeyDownTime = 0.0f;
				LeftMousePressed = true;
			}
		}
	}
}

void AMyCharacter::LeftClickUp()
{
	LeftMousePressed = false;
}

void AMyCharacter::LeftDoubleClick()
{
	LeftMousePressed = false;
	if (SelectedActor)
	{
		FHitResult Hit;
		TArray<TEnumAsByte<EObjectTypeQuery>> oTypes;
		oTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
		PC->GetHitResultUnderCursorForObjects(oTypes, false, Hit);
		if (Hit.GetActor() == SelectedActor)
		{
			SelectedActor->CanMovePiece = true;
			SelectedActor = nullptr;
			ServerChangeTurn(PC->PlayerID);
		}
	}
}

//Rotate Piece
FRotator AMyCharacter::PieceRotation() //Code Snippet from https://answers.unrealengine.com/questions/20858/set-collision-settings-for-brushes.html
{
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	FVector PlaneNormal = FVector(0.0f, 0.0f, 1.0f);
	FVector PlanePoint = SelectedActor->RotationComponent->GetComponentLocation();
	float PlaneFormulaD = -1 * (PlaneNormal.X * PlanePoint.X + PlaneNormal.Y * PlanePoint.Y + PlaneNormal.Z * PlanePoint.Z);
	FVector WorldLocation;
	FVector WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	float t1 = PlaneNormal.X*WorldDirection.X + PlaneNormal.Y*WorldDirection.Y + PlaneNormal.Z*WorldDirection.Z;
	float t2 = -1 * (PlaneFormulaD + PlaneNormal.X*WorldLocation.X + PlaneNormal.Y*WorldLocation.Y + PlaneNormal.Z*WorldLocation.Z);
	float t = t2 / t1;
	FVector IntersectionPoint = FVector(t * WorldDirection.X + WorldLocation.X, t * WorldDirection.Y + WorldLocation.Y, t * WorldDirection.Z + WorldLocation.Z);
	FVector Direction = IntersectionPoint - PlanePoint;
	FRotator Rotation = FRotator(0.0f, FRotator::ClampAxis(Direction.Rotation().Yaw), 0.0f);
	return Rotation;
}

bool AMyCharacter::ServerRotatePiece_Validate(const FRotator& rot, APiecesParent* Piece)
{
	return true;
}

void AMyCharacter::ServerRotatePiece_Implementation(const FRotator& rot, APiecesParent* Piece)
{
	BroadcastRotation(rot, Piece);
}

void AMyCharacter::BroadcastRotation_Implementation(const FRotator rot, APiecesParent* Piece)
{
	Piece->SetActorRotation(rot);
}

//Change Turn
bool AMyCharacter::ServerChangeTurn_Validate(const int& LastTurn)
{
	return true;
}

void AMyCharacter::ServerChangeTurn_Implementation(const int& LastTurn)
{
	AOnlineGameMode* GM = Cast<AOnlineGameMode>(GetWorld()->GetAuthGameMode());
	GM->BroadcastTurn(LastTurn);
}

//Move Piece
bool AMyCharacter::ServerMovePiece_Validate(const FVector& Speed, const int& LastDistance, APiecesParent* Piece)
{
	return true;
}

void AMyCharacter::ServerMovePiece_Implementation(const FVector& Speed, const int& LastDistance, APiecesParent* Piece)
{
	if (LastDistance > 0)
		MovePiece(Speed, LastDistance, Piece);
	else
	{
		MovePiece(Piece->GetActorLocation(), LastDistance, Piece);
		StopMovePiece(Piece);
	}
}

void AMyCharacter::StopMovePiece_Implementation(APiecesParent* Piece)
{
	if (Piece)
	{
		Piece->ChangeOnce = true;
		Piece->CanMovePiece = false;

	}
}

void AMyCharacter::MovePiece_Implementation(const FVector& Speed, const int& LastDistance, APiecesParent* Piece)
{
	FHitResult HitResult;
	if (Piece)
		Piece->SetActorLocation(Speed, true, &HitResult);
	if (HitResult.bBlockingHit)
	{
		APiecesParent* RicochetPiece = Cast<APiecesParent>(HitResult.GetActor());
		if (RicochetPiece)
		{
			if (Piece->Side != RicochetPiece->Side)
			{
				if (Piece->PieceID != 3)
				{
					FVector MirrorDistance = (Piece->DistanceToTravel - Piece->GetActorLocation()).MirrorByVector(HitResult.ImpactNormal);
					FVector Direction = MirrorDistance - Piece->GetActorLocation();
					Piece->SetActorRotation(FRotator(0.0f, FRotator::ClampAxis(Direction.Rotation().Yaw), 0.0f));
					Piece->DistanceToTravel = Piece->GetActorLocation() + FVector(round(Piece->GetActorForwardVector().X * 10) / 10, round(Piece->GetActorForwardVector().Y * 10) / 10, 0.0f) * Piece->Speed;
				}
				else if (Piece->UpgradeOnce)
				{
					FVector MirrorDistance = (Piece->DistanceToTravel - Piece->GetActorLocation()).MirrorByVector(HitResult.ImpactNormal);
					FVector Direction = MirrorDistance - Piece->GetActorLocation();
					Piece->SetActorRotation(FRotator(0.0f, FRotator::ClampAxis(Direction.Rotation().Yaw), 0.0f));
					Piece->DistanceToTravel = Piece->GetActorLocation() + FVector(round(Piece->GetActorForwardVector().X * 10) / 10, round(Piece->GetActorForwardVector().Y * 10) / 10, 0.0f) * Piece->Speed;
				}
				else
				{
					RicochetPiece->Immunity = false;
					RicochetPiece->Shielded = false;
					ServerPieceDamage(RicochetPiece, GetWorld()->GetFirstPlayerController(), Piece);
				}
				ServerPieceDamage(RicochetPiece, GetWorld()->GetFirstPlayerController(), Piece);
			}
			else
			{
				RicochetPiece->SetActorRotation(FRotator(0.0f, FRotator::ClampAxis(Piece->GetActorForwardVector().Rotation().Yaw), 0.0f));
				
				if (RicochetPiece->Speed > LastDistance)
				{
					RicochetPiece->DistanceToTravel = RicochetPiece->GetActorLocation() + FVector(round(floor(RicochetPiece->GetActorForwardVector().X * 10) / 10), round(floor(RicochetPiece->GetActorForwardVector().Y * 10) / 10), 0.0f) * LastDistance;
					RicochetPiece->LastDistance = FMath::FloorToInt((RicochetPiece->GetActorLocation() - RicochetPiece->DistanceToTravel).Size2D());
				}
				else
				{
					RicochetPiece->DistanceToTravel = RicochetPiece->GetActorLocation() + FVector(round(floor(RicochetPiece->GetActorForwardVector().X * 10) / 10), round(floor(RicochetPiece->GetActorForwardVector().Y * 10) / 10), 0.0f) * RicochetPiece->Speed;
					RicochetPiece->LastDistance = FMath::FloorToInt((RicochetPiece->GetActorLocation() - RicochetPiece->DistanceToTravel).Size2D());
				}

				RicochetPiece->CanMovePiece = true;

				FVector MirrorDistance = (Piece->DistanceToTravel - Piece->GetActorLocation()).MirrorByVector(HitResult.ImpactNormal);
				FVector Direction = MirrorDistance - Piece->GetActorLocation();
				Piece->SetActorRotation(FRotator(0.0f, FRotator::ClampAxis(Direction.Rotation().Yaw), 0.0f));
				Piece->DistanceToTravel = Piece->GetActorLocation() + FVector(round(floor(Piece->GetActorForwardVector().X * 10) / 10), round(floor(Piece->GetActorForwardVector().Y * 10) / 10), 0.0f) * Piece->Speed;
			}

			RicochetPiece = nullptr;
			Piece->SetActorEnableCollision(false);
			DisableCollision = Piece;
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMyCharacter::EnableCollision, GetWorld()->GetDeltaSeconds(), false);
		}
		else
		{
			FVector MirrorDistance = (Piece->DistanceToTravel - Piece->GetActorLocation()).MirrorByVector(HitResult.ImpactNormal);
			FVector Direction = MirrorDistance - Piece->GetActorLocation();
			Piece->SetActorRotation(FRotator(0.0f, FRotator::ClampAxis(Direction.Rotation().Yaw), 0.0f));
			Piece->DistanceToTravel = Piece->GetActorLocation() + FVector(round(Piece->GetActorForwardVector().X * 10) / 10, round(Piece->GetActorForwardVector().Y * 10) / 10, 0.0f) * Piece->Speed;
		}
	}
	Piece->LastDistance = LastDistance - 500 * GetWorld()->GetDeltaSeconds();
}



void AMyCharacter::EnableCollision()
{
	if (DisableCollision)
		DisableCollision->SetActorEnableCollision(true);
}

//Collision
bool AMyCharacter::ServerPieceDamage_Validate(APiecesParent* PieceToDamage, APlayerController* PlayerController, APiecesParent* DamageCauser)
{
	return true;
}

void AMyCharacter::ServerPieceDamage_Implementation(APiecesParent* PieceToDamage, APlayerController* PlayerController, APiecesParent* DamageCauser)
{
	UGameplayStatics::ApplyDamage(PieceToDamage, 1.0f, PlayerController, DamageCauser, TSubclassOf<UDamageType>(UDamageType::StaticClass()));
	if(PieceToDamage->Shielded==false)
		BroadcastUpgradePiece(DamageCauser);
}

void AMyCharacter::BroadcastUpgradePiece_Implementation(APiecesParent* Piece)
{
	Piece->Upgraded = true;
}