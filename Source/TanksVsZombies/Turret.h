// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TanksVsZombies.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class ATank;
class AProjectile;

UCLASS()
class TANKSVSZOMBIES_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

	// Called when projectile is spawned
	UFUNCTION(BlueprintImplementableEvent)
	void OnProjectileSpawned();

protected:
	// Max turn rate in degrees/second 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
	float YawSpeed = 180.0f;

	// Parent Tank
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	ATank* Tank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	TSubclassOf<AProjectile> PrimaryProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	TSubclassOf<AProjectile> SecondaryProjectile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Helpful debug tool - which way is our tank facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess))
	class UArrowComponent* TurretDirection;

	// Sprite for the turret body
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess))
	class UPaperSpriteComponent* TurretSprite;

	// Time between each projectile spawn in seconds
	UPROPERTY(EditAnywhere, BlueprintReadONly, Category = "Turret", meta = (AllowPrivateAccess))
	float CoolDown = 0.5f;

	float InitialCoolDown;

	APlayerController* PlayerController;

private:
	void SpawnProjectile(const TSubclassOf<AProjectile>& Projectile);
};
