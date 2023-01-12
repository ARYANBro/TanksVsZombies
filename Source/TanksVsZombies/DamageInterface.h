// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DamageInterface.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Unknown,
	Projectile,
	Crushed,
	ZombieSlap,
	DamageVolume
};

UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class TANKSVSZOMBIES_API IDamageInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetRemainingHealth();
};
