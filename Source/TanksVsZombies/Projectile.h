// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class TANKSVSZOMBIES_API AProjectile : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AProjectile();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void DestroyProjectile();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
	void OnDestroyedProjectile();

	virtual void OnDestroyedProjectile_Implementation();

public:
	// How fast the projectile flies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed = 200.0f;

	// Projectile radius for collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Radius = 20.0f;

	// The amound of damage the projectile inflicts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	int32 DirectDamage = 5.0f;

	// Describle what the projectile hits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName CollisionProfile;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "0.1", AllowPrivateAccess))
	float LifeTime = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Projectile", BlueprintReadWrite)
	class UPaperSpriteComponent* ProjectileSprite;

private:
	FTimerHandle DestroyTimerHandle;
};
