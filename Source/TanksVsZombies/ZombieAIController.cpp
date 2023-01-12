// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"

#include "Zombie.h"
#include "Tank.h"
#include <Kismet/KismetMathLibrary.h>
#include <Components/BoxComponent.h>

void AZombieAIController::Tick(float DeltaTime)
{
	if (!PawnAsZombie)
		return;

	// If zombie should attack, attack else move
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (PawnAsZombie->GetWithinAttackRange())
	{
		if (AttackAvailTime <= CurrentTime)
		{
			PawnAsZombie->AddAttackInput();
			AttackAvailTime = CurrentTime + AttackInterval;
		}
	}
	else
	{   
		AttackAvailTime = CurrentTime + InitialAttackDelay;
		AddRotationAndMovementInput();	
	}
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PawnAsZombie = Cast<AZombie>(GetPawn());
	TargetLocation = GetRandomTargetLocation();
}

void AZombieAIController::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsZombie = nullptr;
}

void AZombieAIController::MoveAway(const TArray<FHitResult>& HitResults)
{
	FVector MovementVector = FVector::ZeroVector;
	for (const FHitResult& HitResult : HitResults)
	{
		MovementVector += HitResult.ImpactNormal;
	}

	MovementVector.Normalize();
	// Move away towards the sum of the impact normals
	float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(PawnAsZombie->GetActorRotation().Yaw, MovementVector.Rotation().Yaw);
	PawnAsZombie->AddRotationInput(DeltaYaw);
}

void AZombieAIController::AddRotationAndMovementInput()
{
	AActor* Target = PawnAsZombie->GetTargetActor();

	// If there is a target go towards it else move to a random point
	if (Target)
		TargetLocation = Target->GetActorLocation();
	else if (float LengthSquared = FVector::DistSquared2D(PawnAsZombie->GetActorLocation(), TargetLocation); LengthSquared <= 50 * 50)
		TargetLocation = GetRandomTargetLocation();

	// Calculate delta yaw desired for moving towards target
	FVector DirectionToTarget = (TargetLocation - PawnAsZombie->GetActorLocation()).GetSafeNormal2D();
	float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorRightVector());
	float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorForwardVector());
	float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);

	// If something is in the way rotate away else rotate towards the target
	TArray<FHitResult> HitResults;
	if (CheckCollision(Target, HitResults))
		MoveAway(HitResults);
	else
		PawnAsZombie->AddRotationInput(DeltaYawDesired);

	PawnAsZombie->AddMovementInput({ 1.0f, 0.0f, 0.0f }, FMath::GetMappedRangeValueClamped(TRange<float>{-0.707f, 0.707f }, TRange<float>{ 0.0f, 1.0f }, DotToTarget));
}

bool AZombieAIController::CheckCollision(AActor* Target, TArray<FHitResult>& OutResults)
{
	// Shouldn't check collision for the controlled zombie or the target
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(PawnAsZombie);
	Params.AddIgnoredActor(Target);

	FCollisionShape ColShape;
	ColShape.SetCapsule(40.0f, 200.0f);
	GetWorld()->SweepMultiByProfile(OutResults, PawnAsZombie->GetActorLocation(), PawnAsZombie->GetActorLocation(), FQuat::Identity, ZombieAvoidProfile, ColShape, Params);

	if (!OutResults.IsEmpty())
	{
		for (FHitResult& HitResult : OutResults)
		{
			if (HitResult.bBlockingHit)
				return true;
		}
	}

	return false;
}

FVector AZombieAIController::GetRandomTargetLocation()
{
	return UKismetMathLibrary::RandomPointInBoundingBox(FVector(ZombieStrollBox.GetCenter(), 0.0f), FVector(ZombieStrollBox.GetExtent(), 0.0f));
}
