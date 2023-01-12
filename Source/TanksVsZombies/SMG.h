// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "SMG.generated.h"

/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API USMG : public UGun
{
	GENERATED_BODY()

public:
	USMG() = default;
	~USMG() = default;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual bool SpawnBullet() override;
	virtual EGunType GetType() override { return EGunType::SMG; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float Firerate = 0.5f;
	
private:
	FTimerHandle TH;
	int32 BulletsSpawned = 0;
	
};
