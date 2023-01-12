// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "DamageInterface.h"

#include <PaperSpriteComponent.h>

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ProjectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ProjectileSprite"));
	ProjectileSprite->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::DestroyProjectile, LifeTime);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();
	FVector DesiredLocation = Location + (Speed * DeltaTime) * GetTransform().GetUnitAxis(EAxis::X);

	if (Radius > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{
			FHitResult OutHit;
			FCollisionShape CollisionShape;
				CollisionShape.SetCapsule(Radius, 200.0f);
			if (World->SweepSingleByProfile(OutHit, Location, DesiredLocation, FQuat::Identity, CollisionProfile, CollisionShape))
			{
				if (OutHit.GetActor()->Implements<UDamageInterface>())
				{
					if (GetActorEnableCollision())
						IDamageInterface::Execute_ReceiveDamage(OutHit.GetActor(), DirectDamage, EDamageType::Projectile);
				}

				SetActorLocation(OutHit.Location, true);
				DestroyProjectile();
			}
			else
				SetActorLocation(DesiredLocation, true);
		}
	}
}

void AProjectile::DestroyProjectile()
{
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
	SetActorEnableCollision(false);
	OnDestroyedProjectile();
}

void AProjectile::OnDestroyedProjectile_Implementation()
{
	Destroy();
}