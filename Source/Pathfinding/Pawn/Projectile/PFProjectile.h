// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "GameFramework/Actor.h"
#include "PFProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class PATHFINDING_API APFProjectile : public AActor
{
	GENERATED_BODY()

public:
	APFProjectile();

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Launch(const FTargetRequest& InRequest);

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(Category = "Projectile", EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "Projectile", EditAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;

	FTargetRequest Request;

	UPROPERTY(Category = "Projectile", EditDefaultsOnly, BlueprintReadOnly)
	FEffectWrapper HitEffect;
	
public:
	static const FName Projectile_ProfileName;
};
