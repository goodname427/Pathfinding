// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/AttackCommandComponent.h"
#include "Components/ActorComponent.h"
#include "AttackerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UAttackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackerComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
protected:
	UFUNCTION()
	void OnAttackCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);
	
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	bool RecursiveCheck();
	
public:
	UPROPERTY()
	UAttackCommandComponent* AttackCommandComponent;

	UPROPERTY()
	AConsciousPawn* Attacker;

protected:
	UPROPERTY(Category = "Attacker", EditDefaultsOnly)
	bool bActiveAttack;
	
	UPROPERTY(Category = "Attacker", EditDefaultsOnly)
	float HuntingRadiusScale;

	UPROPERTY(Category = "Attacker", EditDefaultsOnly)
	int32 MaxRecursiveCountPerFrame = 5;

private:
	int32 RecursiveCount = 0;
};
