// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "Tank.h"
#include "Projectile.h"

#include <Components/ArrowComponent.h>
#include <PaperSpriteComponent.h>
#include <Kismet/GameplayStatics.h>


// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TurretDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TurretDirection"));
	TurretDirection->SetupAttachment(RootComponent);

	TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
	TurretSprite->SetupAttachment(TurretDirection);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	Tank = Cast<ATank>(GetParentActor());
	check(Tank)
	PlayerController = Cast<APlayerController>(Tank->GetController());
	check(PlayerController);
	InitialCoolDown = CoolDown;
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(TurretDirection);

	if (PlayerController)
	{
		TurretDirection->SetWorldRotation(
			UTankStatics::LookTowardsMouseCursor(
				FVector2D(TurretDirection->GetComponentLocation()),
				TurretDirection->GetComponentRotation(),
				PlayerController,
				YawSpeed,
				DeltaTime
			)
		);
	}

	// Handle Input
	const FTankInput& TankInput = Tank->GetCurrentInput();

	CoolDown -= DeltaTime;
	CoolDown = FMath::Clamp(CoolDown, -1.0f, CoolDown);

	if (CoolDown <= 0)
	{
		if (TankInput.bFire1)
		{
			SpawnProjectile(PrimaryProjectile);
			CoolDown = InitialCoolDown;
		}
		else if (TankInput.bFire2)
		{
			SpawnProjectile(SecondaryProjectile);
			CoolDown = InitialCoolDown;
		}
	}
}

void ATurret::SpawnProjectile(const TSubclassOf<AProjectile>& Projectile)
{
	UWorld* World = GetWorld();
	if (!World || !Projectile)
		return;

	AActor* NewProjectile = World->SpawnActor(Projectile);
	if (!NewProjectile)
		return;

	FVector Loc = TurretSprite->GetSocketLocation("Muzzle");
	FRotator Rot = { 0.0f, TurretDirection->GetComponentRotation().Yaw, 90.0f };
	NewProjectile->SetActorLocation(Loc);
	NewProjectile->SetActorRotation(Rot);
	OnProjectileSpawned();
}