// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/AttackCommandComponent.h"
#include "ProjectileAttackCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UProjectileAttackCommandComponent : public UAttackCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UProjectileAttackCommandComponent();

protected:
	virtual void ApplyDamageToTargetPawn_Implementation() override;

protected:
	UPROPERTY(Category = "Command|Projectile", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class APFProjectile> ProjectileClass;
};
