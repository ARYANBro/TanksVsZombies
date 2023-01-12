// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGunType : uint8
{
	None, 
	Pistol,
	SMG,
 	RocketLauncher
};

/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API UGun : public UActorComponent
{
	GENERATED_BODY()

public:
	UGun();
	virtual ~UGun() = default;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/**
	 * @brief Spawns bullet atthe actor location
	 * @return Returns false is couldn't spawn the bullet for some reason
	*/
	virtual bool SpawnBullet();

	UFUNCTION(BlueprintCallable)
	virtual EGunType GetType() { return EGunType::Pistol; }

public:
	bool bFire = false;

protected:
	/* Bullet Actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	TSubclassOf<ABullet> BulletClass;

	// Interval between each fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float Cooldown = 0.5f;

	float InitialCooldown;
};
