// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet.h"
#include "GameFramework/Pawn.h"
#include "Gun.h"
#include "DamageInterface.h"
#include "Soldier.generated.h"

DECLARE_EVENT(ASoldier, FSoldierDiedEventSignature)

class UArrowComponent;
class UPaperFlipbookComponent;

USTRUCT(BlueprintType)
struct FSoldierInput
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	bool bFire1 = false;

	void Sanitise();
	void MoveX(float Value);
	void MoveY(float Value);

private:
	FVector2D RawInput;
};

UCLASS()
class TANKSVSZOMBIES_API ASoldier : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASoldier();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Soldier")
	void ChangeWeaponTo(UGun* Gun);

	virtual void ReceiveDamage_Implementation(int32 IncomingDamage, EDamageType DamageType) override;
	virtual int32 GetRemainingHealth_Implementation() override { return Health; }
	UArrowComponent* GetDirection() const { return Direction; }
	UPaperFlipbookComponent* GetPaperFlipbook() const { return PaperFlipbook; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void SoldierDie(EDamageType DamageType);

	virtual void SoldierDie_Implementation(EDamageType DamageType);

private:
	void MoveX(float Value);
	void MoveY(float Value);
	void Fire1Pressed();
	void Fire1Released();
	void SwitchToPistol();
	void SwitchToSMG();
	void Dash();

public:
	/* Speed at which the soldier moves (units/seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier", meta = (ClampMin = "1.0"))
	float MoveSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier", meta = (ClampMin = "1.0"))
	float YawSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier", meta = (ClampMin = "0.0"))
	int Health = 25;

	UPROPERTY(EditAnywhere, blueprintReadWrite, Category = "Soldier", meta = (ClampMin = "1.0"))
	float DashSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier", meta = (ClampMin = "0.0"))
	float DashCooldown = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier", meta = (ClampMin = "0.0"))
	float DashTime = 0.1f;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UPaperFlipbookComponent* PaperFlipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	class UCapsuleComponent* SoldierCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UArrowComponent* Direction;

	// Bullet actor to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UGun* Pistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	class USMG* SMG;

	FSoldierInput SoldierInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	UGun* ActiveGun = nullptr;

	float InitialMoveSpeed = 2.0f;
	bool bCanDash = true;

	FSoldierDiedEventSignature SoldierDiedEvent;
};
