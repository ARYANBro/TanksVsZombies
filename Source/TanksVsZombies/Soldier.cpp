// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"

#include "TankStatics.h"
#include "SMG.h"
#include "TanksVsZombiesGameModeBase.h"

#include <PaperFlipbookComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include <Components/ArrowComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>

#include <string>

void FSoldierInput::Sanitise()
{
	MovementInput = RawInput;
	MovementInput.Normalize();
	RawInput = FVector2D::ZeroVector;
}

void FSoldierInput::MoveX(float Value)
{
	RawInput.X += Value;
}

void FSoldierInput::MoveY(float Value)
{
	RawInput.Y += Value;
}


// Sets default values
ASoldier::ASoldier()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup components
	SoldierCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SoldierCollision"));
	RootComponent = SoldierCollision;

	Direction = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
	Direction->SetupAttachment(RootComponent);

	PaperFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	PaperFlipbook->SetupAttachment(Direction);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->CameraLagSpeed = 2.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation({ -90.0f, 0.0f, 0.0f });

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1024.0f;
	Camera->AspectRatio = 16.0f / 9.0f;
	Camera->SetupAttachment(SpringArm);

	Pistol = CreateDefaultSubobject<UGun>(TEXT("Gun"));

	SMG = CreateDefaultSubobject<USMG>(TEXT("SMG"));
	SMG->SetComponentTickEnabled(false);

	ActiveGun = Pistol;
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	if (!GetWorld())
		return;

	auto GameMode = Cast<ATanksVsZombiesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return;

	Super::BeginPlay();
	InitialMoveSpeed = MoveSpeed;
	FStopRenderingThreadDelegate Delegate;
	Delegate.BindUFunction(GameMode, "OnPlayerDied");
	SoldierDiedEvent.Add(Delegate);
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
		return;

	Super::Tick(DeltaTime);

	SoldierInput.Sanitise();

	Direction->SetWorldRotation(
		UTankStatics::LookTowardsMouseCursor(
			FVector2D(Direction->GetComponentLocation()),
			Direction->GetComponentRotation(),
			PlayerController,
			YawSpeed,
			DeltaTime
	));

	SetActorLocation(GetActorLocation() + FVector(SoldierInput.MovementInput, 0.0f) * MoveSpeed * DeltaTime, true);
	ActiveGun->bFire = SoldierInput.bFire1;
}

// Called to bind functionality to input
void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveX", this, &ASoldier::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ASoldier::MoveY);
	PlayerInputComponent->BindAction("Fire1", IE_Pressed, this, &ASoldier::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", IE_Released, this, &ASoldier::Fire1Released);
	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ASoldier::SwitchToPistol);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ASoldier::SwitchToSMG);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASoldier::Dash);
}

void ASoldier::ChangeWeaponTo(UGun* Gun)
{
	ActiveGun->SetComponentTickEnabled(false);
	ActiveGun = Gun;
	ActiveGun->SetComponentTickEnabled(true);
}

void ASoldier::ReceiveDamage_Implementation(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			SoldierDie(DamageType);
		}

		return;
	}

	Health -= IncomingDamage;
}

void ASoldier::SoldierDie_Implementation(EDamageType DamageType)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
		PC->RestartLevel();
	
	SoldierDiedEvent.Broadcast();
}

void ASoldier::MoveX(float Value)
{
	SoldierInput.MoveX(Value);
}

void ASoldier::MoveY(float Value)
{
	SoldierInput.MoveY(Value);
}

void ASoldier::Fire1Pressed()
{
	SoldierInput.bFire1 = true;
}

void ASoldier::Fire1Released()
{	
	SoldierInput.bFire1 = false;
}

void ASoldier::SwitchToPistol()
{
	ChangeWeaponTo(Pistol);
}

void ASoldier::SwitchToSMG()
{
	ChangeWeaponTo(SMG);
}

void ASoldier::Dash()
{
	if (!bCanDash)
		return;

	bCanDash = false;
	MoveSpeed = DashSpeed;
	MoveX(SoldierInput.MovementInput.X);
	MoveY(SoldierInput.MovementInput.Y);
	FTimerHandle TH1;
	GetWorldTimerManager().SetTimer(
		TH1,
		TFunction<void ()>([this]()
		{
			MoveSpeed = InitialMoveSpeed;
		}),
		DashTime,
		false
	);

	FTimerHandle TH2;
	GetWorldTimerManager().SetTimer(
		TH2,
		TFunction<void ()>([this]()
		{
			bCanDash = true;
		}),
		DashCooldown,
		false
	);
}
