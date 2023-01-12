// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"

#include "Tank.h"
#include "TanksVsZombiesGameModeBase.h"
#include "Components/CapsuleComponent.h"

#include <Kismet/KismetSystemLibrary.h>

// Sets default values
AZombie::AZombie()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Health = 10.0f;
	SightDistance = 500.0f;
	SightAngle = 60.0f;
	YawSpeed = 90.0f;
	WalkSpeed = 25.0f;
	RunSpeed = 45.0f;
	AttackDistance = 100.0f;
	AttackAngle = 30.0f;
	AttackCooldown = 1.0f;

	ZombieCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ZombieCapsule"));
	ZombieCapsule->SetWorldRotation({ -90.0f, 0.0f, 0.0f});
	ZombieCapsule->SetCapsuleHalfHeight(200.0f);
	ZombieCapsule->SetCapsuleRadius(22.0f);
	RootComponent = ZombieCapsule;
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZombieAI(DeltaTime);
	ConsumeMovementInputVector();
	ConsumeRotationInput();
	ConsumeAttackInput();
}

// Called to bind functionality to input
void AZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AZombie::SetTarget(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

void AZombie::UpdateLocationAndRotation(float DeltaTime)
{
	FVector DeltaLocation = bWithinAttackRange ? FVector::ZeroVector : (FMath::GetMappedRangeValueClamped(TRange<double>(0.0f, 1.0f), TRange<double>(WalkSpeed, RunSpeed), GetPendingMovementInputVector().X)) * DeltaTime *  GetActorForwardVector();
	FVector DesiredLocation = GetActorLocation() + DeltaLocation;
	float MaxYawThisFrame = YawSpeed * DeltaTime;
	FRotator DeltaRotation = FRotator(0.0f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.0f);
	FRotator DesiredRot = GetActorRotation() + DeltaRotation;
	SetActorLocationAndRotation(DesiredLocation, DesiredRot.Quaternion(), true);
}

void AZombie::HandleAttackInput(AActor* Target)
{
	APawn* TargetPawn = Cast<APawn>(Target);
	if (!TargetPawn)
		return;

	if (GetAttackInput())
	{
		InflictDamageOnTarget(TargetPawn, 10);
		if (APlayerController* PC = Cast<APlayerController>(TargetPawn->GetController()))
			PC->ClientStartCameraShake(HitShake, CameraShakeIntensity, ECameraShakePlaySpace::CameraLocal);
	}
}

void AZombie::FindTarget()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetCapsule(SightDistance, 200.0f);
	World->SweepMultiByProfile(HitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, SightProfile, CollisionShape);

	for (const FHitResult& HR : HitResults)
	{
		if (AActor* HitActor = HR.GetActor(); HitActor && HitActor->ActorHasTag("Player"))
		{
			if (auto GameMode = Cast<ATanksVsZombiesGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				FStopRenderingThreadDelegate Delegate;
				Delegate.BindUFunction(this, "OnPlayerDied");
				GameMode->AddOnPlayerDiedDelegate(Delegate);
			}

			FVector DirToTarget = (HitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirToTarget, GetActorForwardVector());
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(SightAngle)))
				SetTarget(HitActor);
		}
	}
}

void AZombie::InflictDamageOnTarget(AActor* Target, int DamageInflicted)
{
	if (!Target->Implements<UDamageInterface>())
		return;

	if (IDamageInterface::Execute_GetRemainingHealth(Target) >= 0)
	{
		ZombieAttack(GetWorld()->GetDeltaSeconds());
		IDamageInterface::Execute_ReceiveDamage(Target, 10.0f, EDamageType::ZombieSlap);
	}
}

void AZombie::ZombieAI_Implementation(float DeltaTime)
{
	bWithinAttackRange = WithinAttackRange();
	FVector OriginalLoc = GetActorLocation();
	UpdateLocationAndRotation(DeltaTime);
	if (FVector DistanceWalked = GetActorLocation() - OriginalLoc; !DistanceWalked.IsNearlyZero())
	{
		if (!GetTargetActor())
			ZombieWalk(DeltaTime, DistanceWalked);
		else 
			ZombieRun(DeltaTime);
	}

	if (AActor* Target = GetTargetActor())
		HandleAttackInput(Target);	
	else
		FindTarget();	
}

bool AZombie::WithinAttackRange_Implementation()
{
	AActor* Target = GetTargetActor();
	if (!Target)
		return false;

	FVector OurLocation = GetActorLocation();
	FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal2D();
	float DotToTarget = FVector::DotProduct(DirectionToTarget, GetActorForwardVector());
	float DistanceSquaredXY = FVector::DistSquared2D(Target->GetActorLocation(), OurLocation);
	bool bWithinAttackAngle = DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle));
	bool bWithinAttackDistance = DistanceSquaredXY <= (AttackDistance * AttackDistance);
	return bWithinAttackAngle && bWithinAttackDistance;
}

void AZombie::ZombieDie_Implementation(EDamageType DamageType)
{
	Destroy();
}

void AZombie::ReceiveDamage_Implementation(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= Health)
	{
		if (Health >= 0)
		{
			Health = -1;
			ZombieDie(DamageType);
		}

		return;
	}

	Health -= IncomingDamage;
}

void AZombie::AddRotationInput(float DeltaYawDegrees)
{
	YawInput += DeltaYawDegrees;
}

void AZombie::AddAttackInput()
{
	bAttackInput = true;
}

bool AZombie::ConsumeAttackInput()
{
	bool bRetValue = bAttackInput;
	bAttackInput = false;
	return bRetValue;
}

float AZombie::ConsumeRotationInput()
{
	float RetValue = YawInput;	
	YawInput = 0.0f;
	return RetValue;
}

void AZombie::OnPlayerDied()
{
	TargetActor = nullptr;
}
