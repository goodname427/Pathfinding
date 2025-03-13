// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAttackCommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Projectile/PFProjectile.h"


UProjectileAttackCommandComponent::UProjectileAttackCommandComponent()
{
	Data.RequiredTargetRadius = 500;
}

void UProjectileAttackCommandComponent::ApplyDamageToTargetPawn_Implementation()
{
	UWorld* World = GetWorld();
	NULL_CHECK(World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetExecutePawn();
	SpawnParams.Instigator = GetExecutePawn();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	APFProjectile* Projectile = World->SpawnActor<APFProjectile>(ProjectileClass, GetExecutePawn()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (Projectile)
	{
		Projectile->Launch(Request);
	}
}

