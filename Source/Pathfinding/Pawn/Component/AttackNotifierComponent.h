// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsciousPawn.h"
#include "Components/ActorComponent.h"
#include "AttackNotifierComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UAttackNotifierComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttackNotifierComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY()
	AConsciousPawn* OwnerPawn;

	UPROPERTY(Category = "AttackNotifier", EditAnywhere, BlueprintReadOnly)
	float NotifyRadius;
};
