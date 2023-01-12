// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageVolume.generated.h"

UCLASS()
class TANKSVSZOMBIES_API ADamageVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageVolume();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageVolume")
	bool bEnabled = false;

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DamageVolume;

	UPROPERTY(EditAnywhere, Category = "DamageVolume")
	FName TargetProfile;

	UPROPERTY(EditAnywhere, Category = "DamageVolume")
	int Damage = 0;
};
