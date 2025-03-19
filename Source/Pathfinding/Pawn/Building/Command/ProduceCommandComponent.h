// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Command/PeriodicProgressCommandComponent.h"
#include "Command/ProgressCommandComponent.h"
#include "ProduceCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UProduceCommandComponent : public UPeriodicProgressCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UProduceCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	float GetProducedCountPerSecond() const { return ProducedCountPerCycle / GetProgressDuration(); }

protected:
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

public:
	UPROPERTY(Category = "Command|Produce", EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 ProducedCountPerCycle;	
};
