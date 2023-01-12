// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TanksVsZombies.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Soldier.h"
#include "Tank.generated.h"

DECLARE_EVENT(ATank, FTankDiedEventSignature)

// This struct covers all possible tank input schemes
// What the inputs do can vary by tank, but the same inputs will always exist
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_BODY()
	
	// Sanitised movement input, ready to be used by game logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	uint32 bFire1 : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	uint32 bFire2 : 1;

	void Sanitise();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1(bool bPressed);
	void Fire2(bool bReleased);

private:
	// Private because it's internal, raw data. Game code should never see this
	FVector2D RawMovementInput;
};

UCLASS()
class TANKSVSZOMBIES_API ATank : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//~ Begin IDamageInterface
	virtual void ReceiveDamage_Implementation(int32 Damage, EDamageType DamageType) override;
	virtual int32 GetRemainingHealth_Implementation() override { return Health; }
	//~ End IDamageInterface

	// Called when the tank dies
	UFUNCTION(BlueprintNativeEvent, Category = "Tank")
	void TankDie(EDamageType DamageType);
	virtual void TankDie_Implementation(EDamageType DamageType);

	UFUNCTION(BlueprintCallable, Category = "Tank")
	const FTankInput& GetCurrentInput() const { return TankInput; }

	template<typename FunctorType, typename... VarTypes>
	void AddOnTankDiedDelegate(FunctorType&& InFunctor, VarTypes... Vars) { TankDiedEvent.AddLambda(InFunctor, Vars...); }
private:
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void Fire1Pressed();
	void Fire1Released();
	void Fire2Pressed();
	void Fire2Released();
	void MoveTank(float DeltaTime);
	void CrushZombies();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TankInput")
	FTankInput TankInput;

	// Maximum turn rate (degrees/second) of the tank.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "1.0"))
	float YawSpeed = 1.0f;

	// Maximum movement rate (units/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "1.0"))
	float MoveSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	int32 Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank")
	FName CollisionProfile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tank")
	TSubclassOf<ASoldier> SoldierClass;

private:
	// Helpful debug tool - which way is our tank facing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess))
	class UArrowComponent* TankDirection;

	// Sprite for the tank body
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess))
	class UPaperSpriteComponent* TankSprite;

	// The actor used as turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess))
	UChildActorComponent* ChildTurret;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowprivateAccess))
	class UCameraComponent* CameraComponent;

	// Collision body for the tank
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowprivateAccess))
	class UBoxComponent* TankBody;

	FTankDiedEventSignature TankDiedEvent;

};
