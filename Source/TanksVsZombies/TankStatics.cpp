// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStatics.h"

#include <GameFramework/Actor.h>
#include <PaperFlipbookComponent.h>
#include <PaperFlipbook.h>

float UTankStatics::FindDeltaAngleDegrees(float Angle1, float Angle2)
{
    // Find the difference
    float Delta = Angle2 - Angle1;
    if (Delta > 180.0f)
        Delta  = Delta - 360.0f;
    else if (Delta < -180.0f)
        Delta = Delta + 360.0f;

    return Delta;
}

bool UTankStatics::FindLookatAngle2D(const FVector2D& Start, const FVector2D& Target, float& OutAngle)
{
    FVector2D Normal = (Target - Start);
    if (!Normal.IsNearlyZero())
    {
        OutAngle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
        return true;
    }

    return false;
}

void UTankStatics::PutInZPlane(AActor* InActor)
{
    InActor->SetActorRotation({ 90.0f, 0.0f, 0.0f });
}

bool UTankStatics::PlayFlipbook(UPaperFlipbookComponent* PaperFlipbookComponent, UPaperFlipbook* NewFlipbook, bool bLooping, float PlayRate)
{
    if (PaperFlipbookComponent && NewFlipbook)
    {
        PaperFlipbookComponent->SetFlipbook(NewFlipbook);
        PaperFlipbookComponent->SetLooping(bLooping);
        PaperFlipbookComponent->SetPlayRate(PlayRate);
        PaperFlipbookComponent->Play();
        return true;
    }

    return false;
}

FRotator UTankStatics::LookTowardsMouseCursor(FVector2D ActorLocation, FRotator ActorRotation, const APlayerController* PlayerController, float YawSpeed, float DeltaTime)
{
	if (FVector2D CursorPosition; PlayerController->GetMousePosition(CursorPosition.X, CursorPosition.Y))
	{
		if (PlayerController)
		{
			// Get the TurretLocation and the cursor in the same space (screen)
			if (FVector MousePos3D, MouseDir; PlayerController->DeprojectMousePositionToWorld(MousePos3D, MouseDir))
			{
				FVector2D MousePos2D = FVector2D(MousePos3D);

				// Calculate the angle to the cursor
				if (float DesiredYaw; UTankStatics::FindLookatAngle2D(ActorLocation, MousePos2D, DesiredYaw))
				{
					// Find the amt of degrees we have to yaw
					float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(ActorRotation.Yaw, DesiredYaw);
					float MaxDeltaYawThisFrame = YawSpeed * DeltaTime;

					if (MaxDeltaYawThisFrame >= FMath::Abs(DeltaYaw))
					{
						// We can get there in one frame so just get there
						ActorRotation.Yaw = DesiredYaw;
					}
					else
					{
						// We can't get there in one frame so add the max yaw we can move this frame to current rotation
						ActorRotation.Yaw += FMath::Sign(DeltaYaw) * MaxDeltaYawThisFrame;
					}

					return ActorRotation;
				}
			}
		}

		
	}

	return FRotator::ZeroRotator;
}