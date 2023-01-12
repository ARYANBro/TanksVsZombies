// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "Soldier.h"

#include "Components/ArrowComponent.h"
#include "PaperFlipbookComponent.h"

UGun::UGun()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UGun::BeginPlay()
{
    Super::BeginPlay();
    InitialCooldown = Cooldown;
}

void UGun::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Cooldown -= DeltaTime;
    Cooldown = FMath::Clamp(Cooldown, -1.0f, Cooldown);

    if (Cooldown <= 0.0f && bFire)
    {
        if (!SpawnBullet())
            UE_LOG(LogTemp, Error, TEXT("Couldn't Spawn bullet"));
        
        Cooldown = InitialCooldown;
    }
}

bool UGun::SpawnBullet()
{
    UWorld* World = GetWorld();
    if (!World)
        return false;

    if (!BulletClass)
        return false;

    AActor* Bullet = World->SpawnActor(BulletClass);
    if (!Bullet)
        return false;

    if (ASoldier* Soldier = Cast<ASoldier>(GetOwner()); Soldier && Soldier->GetPaperFlipbook()->DoesSocketExist("Muzzle"))
    {
        FVector Location = Soldier->GetPaperFlipbook()->GetSocketLocation("Muzzle");
        FRotator Rotation = { 0.0f, Soldier->GetDirection()->GetComponentRotation().Yaw, 90.0f };
        Bullet->SetActorLocation(Location);
        Bullet->SetActorRotation(Rotation);
        return true;
    }

    return false;
}
