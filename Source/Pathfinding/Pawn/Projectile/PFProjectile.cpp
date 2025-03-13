// Fill out your copyright notice in the Description page of Project Settings.


#include "PFProjectile.h"

#include "PFUtils.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

const FName APFProjectile::Projectile_ProfileName = FName("Projectile");

// Sets default values
APFProjectile::APFProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = INIT_DEFAULT_SUBOBJECT(StaticMeshComponent);
	StaticMeshComponent->SetCollisionProfileName(Projectile_ProfileName);
	if (GetLocalRole() == ROLE_Authority)
	{
		StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	}

	INIT_DEFAULT_SUBOBJECT(ProjectileMovementComponent);
	ProjectileMovementComponent->InitialSpeed = 1000;
	ProjectileMovementComponent->MaxSpeed = 1000;

	InitialLifeSpan = 10;
}

void APFProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (APFPawn* OtherPawn = Cast<APFPawn>(OtherActor))
	{
		if (const APFPawn* PawnInstigator = GetInstigator<APFPawn>())
		{
			if (OtherPawn != PawnInstigator && (!Request.IsTargetPawnValid() || OtherPawn == Request.TargetPawn))
			{
				UGameplayStatics::ApplyDamage(
					OtherPawn,
					PawnInstigator->GetAttack(),
					GetInstigatorController(),
					this,
					UDamageType::StaticClass()
				);

				PlayEffect(this, HitEffect);
				
				Destroy();
			}
		}
	}
}

void APFProjectile::Launch_Implementation(const FTargetRequest& InRequest)
{
	Request = InRequest;

	FVector Direction = InRequest.GetTargetLocation() - GetActorLocation();
	Direction.Z = 0;
	Direction.Normalize();

	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}
