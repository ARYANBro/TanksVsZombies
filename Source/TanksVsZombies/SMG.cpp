// Fill out your copyright notice in the Description page of Project Settings.


#include "SMG.h"

void USMG::BeginPlay()
{
	Super::BeginPlay();
	Cooldown = Firerate;
}

void USMG::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    UGun::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool FiredBullets = BulletsSpawned >= 3;

	// If we've just started to shoot a bullet
	if (FMath::IsNearlyEqual(Cooldown, InitialCooldown))
	{
		// And we've not fired all the bullets so set fire at the rate of "Firerate"
		if (!FiredBullets)
			Cooldown = Firerate;
	}

    if (FiredBullets)
	{
		GetWorld()->GetTimerManager().ClearTimer(TH);
		BulletsSpawned = 0;
	}
}

bool USMG::SpawnBullet()
{
	if (!UGun::SpawnBullet())
		return false;

	BulletsSpawned++;
	return true;
}
