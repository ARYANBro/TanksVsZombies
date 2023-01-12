// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TankStatics.generated.h"

/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API UTankStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Calculate th angle difference in range [-180, 180]
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float Angle1, float Angle2);

	// Finds the angle beweetn two points
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (Keywords = "rotation rotate"))
	static bool FindLookatAngle2D(const FVector2D& Start, const FVector2D& Target, float& OutAngle);

	UFUNCTION(BlueprintCallable, Category = "Uility")
	static void PutInZPlane(AActor* InActor);

	// Returns false if cannot play the flipbook
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static bool PlayFlipbook(class UPaperFlipbookComponent* PaperFlipbookComponent, class UPaperFlipbook* NewFlipbook, bool bLooping, float PlayRate = 1.0f);

	// Returns the delta rotation to look towards the cursor
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static FRotator LookTowardsMouseCursor(FVector2D ActorLocation, FRotator ActorRotation, const APlayerController* PlayerController, float YawSpeed, float DeltaTime);
};
