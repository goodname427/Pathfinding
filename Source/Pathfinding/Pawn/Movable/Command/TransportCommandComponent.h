﻿// Fill out your copyright notice in the Description page of Project Settings.

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
	
protected:
	virtual bool InternalIsReachable_Implementation() override;
	
	virtual void InternalBeginExecute_Implementation() override;


	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;
};