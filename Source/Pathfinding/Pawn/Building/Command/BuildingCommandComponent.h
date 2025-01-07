// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Command/ProgressCommandComponent.h"
#include "BuildingCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UBuildingCommandComponent : public UProgressCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBuildingCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	virtual bool InternalIsCommandEnable_Implementation() const override;
	
	virtual float GetProgressDuration_Implementation() const override;

	virtual UObject* GetCommandIcon_Implementation() const override;
	
public:
	virtual void InternalPushedToQueue_Implementation() override;
	
	virtual void InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason) override;

	virtual void InternalBeginExecute_Implementation() override;
	
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;;
};
