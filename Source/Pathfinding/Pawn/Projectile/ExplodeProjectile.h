// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFProjectile.h"
#include "ExplodeProjectile.generated.h"

UCLASS()
class PATHFINDING_API AExplodeProjectile : public APFProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplodeProjectile();

protected:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	UPROPERTY(Category = "Projectile", EditDefaultsOnly, BlueprintReadOnly)
	float ExplodeRadius;
};
