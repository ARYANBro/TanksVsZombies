// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include <Camera/CameraShakeBase.h>

#include "Zombie.generated.h"

UCLASS()
class TANKSVSZOMBIES_API AZombie : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZombie();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTarget(AActor* NewTarget);

	// Returns the target actor. Returning null indicates no target
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const { return TargetActor; }

	bool GetWithinAttackRange() const { return bWithinAttackRange; }

	// Zombie will call this on tick
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void ZombieAI(float DeltaTime);
	virtual void ZombieAI_Implementation(float DeltaTime);

	// This function asks the zombie if it's in the position to attack its current target.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	bool WithinAttackRange();
	virtual bool WithinAttackRange_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieWalk(float DeltaTime, FVector DistanceWalkedThisFrame);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieAttack(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieRun(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void ZombieDie(EDamageType DamageType);	
	virtual void ZombieDie_Implementation(EDamageType DamageType);
	virtual void ReceiveDamage_Implementation(int32 IncomingDamage, EDamageType DamageType) override;
	virtual int32 GetRemainingHealth_Implementation() override { return Health; }

	// Add rotation equal to the yaw (in degrees) provided
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual void AddRotationInput(float DeltaYawDegrees);

	// Press the attack button
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual void AddAttackInput();

	virtual bool GetAttackInput() const { return bAttackInput; }
	float GetYawSpeed() const { return YawSpeed; }

	// return and clear the attack input. pre-clear
	virtual bool ConsumeAttackInput();

	// Get (and clear) rotation input. Returns pre-clear value
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual float ConsumeRotationInput();
	
	// Get rotation input. Returns pre-clear value
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	virtual float GetRotationInput() { return YawInput; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPlayerDied();
	void UpdateLocationAndRotation(float DeltaTime);
	void HandleAttackInput(AActor* Target);
	void FindTarget();
	void InflictDamageOnTarget(AActor* Target, int DamageInflicted);

protected:
	friend class AZombieAIController;

	// Camera effect, if any, to play when the player is hit by this Zombie
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> HitShake;	

	// Current health value.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	int32 Health;

	// Sight distance (when not target is present)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightDistance;

	// Sight have-angle in degrees (when no target is present)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightAngle;

	// Max yaw speed per second in degrees
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float YawSpeed;

	// Speed when wandering around, or when facing away from the target
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float WalkSpeed;

	// Speed when rushing towards the target
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float RunSpeed;

	// Attack distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackDistance;

	// Half-Angle (in degrees) for our attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackAngle;

	// Minimum time between attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackCooldown;

	// Game time, in seconds, when the zombie will be allowed to attack again
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Zombie")
	float AttackAvailableTime;

	UPROPERTY(Editanywhere)
	FName SightProfile;

	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* ZombieCapsule;

private:
	// The actor we are targetting, will be null if there is no target
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "Zombie", meta = (Clampmin = "1.0"))
	float CameraShakeIntensity = 5.0f;

	// Current rotation input.
	float YawInput;

	// Current attack button status
	bool bAttackInput;

	// If in attack range (angle/distance)
	bool bWithinAttackRange;
};
