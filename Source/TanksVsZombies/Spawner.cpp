// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

#include "TankStatics.h"

#include <Components/BoxComponent.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	FVector BoxExtent = SpawnVolume->GetUnscaledBoxExtent();
	BoxExtent.Z = 100.0f;
	SpawnVolume->SetBoxExtent(BoxExtent, false);

	if (RootComponent)
		SpawnVolume->SetupAttachment(RootComponent);
	else
		RootComponent = SpawnVolume;
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ActorToSpawn)
	{
		if (UWorld* World = GetWorld())
		{
			FCollisionShape CollisionShape;
			CollisionShape.SetBox(FVector3f(SpawnVolume->GetUnscaledBoxExtent()));
			if (!World->OverlapAnyTestByProfile(
				SpawnVolume->GetComponentLocation(),
				SpawnVolume->GetComponentRotation().Quaternion(),
				SpawnCollisionProfile,
				CollisionShape
			))
			{
				if (!GetWorldTimerManager().IsTimerActive(TimerHandle))
					GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpawner::SpawnActor, SpawnInterval);
			}
		}
	}
}

void ASpawner::SpawnActor()
{
	if (AActor* SpawnedActor = GetWorld()->SpawnActor(ActorToSpawn->GetDefaultObject()->GetClass()))
	{
		FVector RandPoint = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->GetComponentLocation(), SpawnVolume->GetUnscaledBoxExtent());
		SpawnedActor->SetActorLocation({
			RandPoint.X,
			RandPoint.Y,
			10.0f
		});

		SpawnedActor->SetActorRotation(GetActorRotation().Quaternion());
	}
}
