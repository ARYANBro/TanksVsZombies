// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

#include "Zombie.h"
#include "TanksVsZombiesGameModeBase.h"

#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <PaperSpriteComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>

void FTankInput::Sanitise()
{
	MovementInput = RawMovementInput.ClampAxes(-1.0f, 1.0f);
	MovementInput.Normalize();
	RawMovementInput.Set(0.0f, 0.0f);
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FTankInput::Fire1(bool bPressed)
{
	bFire1 = bPressed;	
}

void FTankInput::Fire2(bool bPressed)
{
	bFire2 = bPressed;
}

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting up components
	TankBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TankBody"));
	TankBody->SetupAttachment(TankDirection);
	TankBody->SetBoxExtent({ 40.0f, 40.0f, 100.0f });
	RootComponent = TankBody;

	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);

	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->SetupAttachment(TankDirection);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation({ -90.0f, 0.0f, 0.0f });

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 16.0f / 9.0f;
	CameraComponent->SetupAttachment(SpringArm);
	CameraComponent->PostProcessSettings = FPostProcessSettings();
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	if (!GetWorld())
		return;

	auto GameMode = Cast<ATanksVsZombiesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Either GameModeBase is not \"ATanksVsZombiesGameModeBase\" or GameModeBase is null"));
		return;
	}

	Super::BeginPlay();

	FStopRenderingThreadDelegate OnPlayerDied;
	OnPlayerDied.BindUFunction(GameMode, "OnPlayerDied");
	TankDiedEvent.Add(OnPlayerDied);
}
// Called every frame
void ATank::Tick(float DeltaTime)
{
	if (IDamageInterface::Execute_GetRemainingHealth(this) <= 0)
		return;

	Super::Tick(DeltaTime);
	TankInput.Sanitise();
	MoveTank(DeltaTime);
	CrushZombies();
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveX"), this, &ATank::MoveX);
	PlayerInputComponent->BindAxis(TEXT("MoveY"), this, &ATank::MoveY);
	PlayerInputComponent->BindAction(TEXT("Fire1"), IE_Pressed, this, &ATank::Fire1Pressed);
	PlayerInputComponent->BindAction(TEXT("Fire1"), IE_Released, this, &ATank::Fire1Released);
	PlayerInputComponent->BindAction(TEXT("Fire2"), IE_Pressed, this, &ATank::Fire2Pressed);
	PlayerInputComponent->BindAction(TEXT("Fire2"), IE_Released, this, &ATank::Fire2Released);
}

void ATank::ReceiveDamage_Implementation(int32 Damage, EDamageType DamageType)
{
	if (Damage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			TankDie(DamageType);
		}

		return;
	}

	Health -= Damage;
}

void ATank::TankDie_Implementation(EDamageType DamageType)
{
	FActorSpawnParameters SpawnParams = {};
	SpawnParams.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	if (APawn* SoldierPawn = Cast<APawn>(GetWorld()->SpawnActor(SoldierClass, &Location, &Rotation, SpawnParams)))
	{
		GetWorld()->GetFirstPlayerController()->Possess(SoldierPawn);
		TankDiedEvent.Broadcast();
		Destroy();
	}
}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);	
}

void ATank::Fire1Pressed()
{
	TankInput.Fire1(true);
}

void ATank::Fire1Released()
{
	TankInput.Fire1(false);
}

void ATank::Fire2Pressed()
{
	TankInput.Fire2(true);
}

void ATank::Fire2Released()
{
	TankInput.Fire2(false);
}

void ATank::MoveTank(float DeltaTime)
{
	if (TankInput.MovementInput.IsNearlyZero())
		return; 

	// Move the tank
	FRotator TankRotation = TankDirection->GetComponentRotation();
	FRotator ToRotate = FVector(TankInput.MovementInput, 0.0f).Rotation();
	float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TankRotation.Yaw, ToRotate.Yaw);

	// Rotate to the closest corrosponding angle
	if (DeltaYaw < -90.0f)
	{
		DeltaYaw += 180.0f;	
		ToRotate.Yaw += 180.0f;
	}
	else if (DeltaYaw > 90.0f)
	{
		DeltaYaw -= 180.0f;
		ToRotate.Yaw += 180.0f;
	}

	if (FMath::Abs(DeltaYaw) <= DeltaTime * YawSpeed)
		TankDirection->SetWorldRotation(ToRotate);
	else
		TankDirection->AddLocalRotation({ 0.0f, FMath::Sign(DeltaYaw) * DeltaTime * YawSpeed, 0.0f });

	SetActorLocation(GetActorLocation() + FVector(TankInput.MovementInput, 0.0f) * MoveSpeed * DeltaTime, true);
}

void ATank::CrushZombies()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("void ATank::CrushZombies() UWorld* World = GetWorld() is nullptr"));
		return;
	}

	// Get all the actor in range
	TArray<FHitResult> HitResults;
	FVector BoxSize = TankBody->GetScaledBoxExtent();
	FCollisionShape CollisionShape;
	CollisionShape.SetBox(FVector3f(BoxSize));
	World->SweepMultiByProfile(HitResults, GetActorLocation(), GetActorLocation(), TankBody->GetComponentRotation().Quaternion(), CollisionProfile, CollisionShape);

	for (const FHitResult& HitResult : HitResults)
	{
		// Check if actor implements DamageInterface
		if (HitResult.GetActor()->Implements<UDamageInterface>())
		{
			// Deal the maximum amount of damage we can inflict
			int32 TargetHealth = IDamageInterface::Execute_GetRemainingHealth(HitResult.GetActor());
			if (TargetHealth >= 0)
				IDamageInterface::Execute_ReceiveDamage(HitResult.GetActor(), TargetHealth, EDamageType::Crushed);
		}
	}
}
