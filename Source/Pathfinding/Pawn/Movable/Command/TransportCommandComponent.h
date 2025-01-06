// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "TransportCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UTransportCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTransportCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	bool CanTransport(const ABaseCampPawn* BaseCamp) const;
	
protected:
	virtual bool InternalIsCommandEnable_Implementation() const override;
	
	virtual bool InternalIsArgumentsValid_Implementation() const override;
	
	virtual void InternalBeginExecute_Implementation() override;
};
