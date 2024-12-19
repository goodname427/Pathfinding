// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conscious/CommandComponent.h"
#include "TransportCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UTransportCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTransportCommandComponent();

public:
	IMPL_GET_COMMAND_NAME()
	
protected:
	virtual bool InternalIsReachable_Implementation() override;
	
	virtual void InternalBeginExecute_Implementation() override;


	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

public:
	static FName CommandName;
};
