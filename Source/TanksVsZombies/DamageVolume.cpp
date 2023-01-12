// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageVolume.h"
#include "DamageInterface.h"
#include <Components/BoxComponent.h>

// Sets default values
ADamageVolume::ADamageVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DamageVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Volume"));
	RootComponent = DamageVolume;
}

// Called when the game starts or when spawned
void ADamageVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADamageVolume::Tick(float DeltaTime)
{
	if (!bEnabled)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;
		
	Super::Tick(DeltaTime);
	TArray<FOverlapResult> OutOverlaps = {};
	World->OverlapMultiByProfile(OutOverlaps, GetActorLocation(), GetActorRotation().Quaternion(), TargetProfile, DamageVolume->GetCollisionShape());
	for (const FOverlapResult& OutOverlap : OutOverlaps)
	{
		if (AActor* OverlapActor = OutOverlap.GetActor(); OverlapActor->Implements<UDamageInterface>())
			IDamageInterface::Execute_ReceiveDamage(OverlapActor, Damage, EDamageType::DamageVolume);
	}
}