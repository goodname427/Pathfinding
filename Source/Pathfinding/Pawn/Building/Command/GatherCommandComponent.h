// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "GatherCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UGatherCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UGatherCommandComponent();

public:
	DECLARE_COMMAND_NAME()
	
	UPROPERTY()
	UStaticMeshComponent* GatherFlagMeshComponent;

protected:
	virtual void InternalBeginExecute_Implementation() override;
};
