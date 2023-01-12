// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	// Moves away from the colliding objects
	void MoveAway(const TArray<FHitResult>& HitResult);

	// Adds rotation and movement input to the zombie
	void AddRotationAndMovementInput();

	// Checks for collision for zombie to manuver
	bool CheckCollision(AActor* Target, TArray<FHitResult>& OutResults);

	// Gets random location within the bounding box
	FVector GetRandomTargetLocation();

protected:
	// Our pawn, pre-cast to a Zombie, Will be null is pawn is null or is not a Zombie
	UPROPERTY(BlueprintReadOnly, Category = "Zombie Controller")
	class AZombie* PawnAsZombie;

	// Profile which includes what objects the zombie will manuver
	UPROPERTY(EditAnywhere, Category = "Zombie Controller")
	FName ZombieAvoidProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Controller")
	FBox2D ZombieStrollBox;

	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Zombie Controller", meta = (Clampmin = "0.0"))
	float AttackInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Zombie Controller", meta = (Clampmin = "0.0"))
	float InitialAttackDelay = 0.3f;

	float AttackAvailTime = 0.0f;
};