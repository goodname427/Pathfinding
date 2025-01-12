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

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnAttackCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);

	UFUNCTION()
	void OnAttackCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason);

public:
	UPROPERTY()
	UAttackCommandComponent* AttackCommandComponent;

	UPROPERTY()
	AConsciousPawn* Attacker;
};
