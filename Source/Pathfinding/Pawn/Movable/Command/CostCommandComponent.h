// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/PeriodicProgressCommandComponent.h"
#include "CostCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UCostCommandComponent : public UPeriodicProgressCommandComponent
{
	GENERATED_BODY()

public:
	UCostCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	float GetCostCountPerSecond() const { return CostCountPerCycle / GetProgressDuration(); }

protected:
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

protected:
	UPROPERTY(Category = "Command|Cost", EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 CostCountPerCycle;

	UPROPERTY(Category = "Command|Cost", EditDefaultsOnly, BlueprintReadOnly)
	int32 HungerDamage = 1;
};
