// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodeProjectile.h"

#include "PFBlueprintFunctionLibrary.h"


// Sets default values
AExplodeProjectile::AExplodeProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplodeRadius = 50;
}

void AExplodeProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const APFPawn* OtherPawn = Cast<APFPawn>(OtherActor))
	{
		if (const APFPawn* PawnInstigator = GetInstigator<APFPawn>())
		{
			if (OtherPawn == Request.TargetPawn)
			{
				static TArray<APFPawn*> AroundPawns;
				UPFBlueprintFunctionLibrary::GetAroundPawns(PawnInstigator, GetActorLocation(), AroundPawns, ExplodeRadius, EPawnRole::Enemy);
				for (const auto& Pawn : AroundPawns)
				{
					UGameplayStatics::ApplyDamage(
						Pawn,
						PawnInstigator->GetAttack(),
						PawnInstigator->GetController(),
						this,
						UDamageType::StaticClass()
					);
				}

				PlayEffect(this, HitEffect);
				
				Destroy();
			}
		}
	}
}

