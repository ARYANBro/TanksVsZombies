// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Missile.generated.h"

UCLASS()
class TANKSVSZOMBIES_API AMissile : public AProjectile
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMissile();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	class UPaperFlipbookComponent* Flipbook;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
