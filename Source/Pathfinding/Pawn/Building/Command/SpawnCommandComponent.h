// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Command/ProgressCommandComponent.h"
#include "SpawnCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API USpawnCommandComponent : public UProgressCommandComponent
{
	GENERATED_BODY()

public:
	USpawnCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	virtual float GetProgressDuration_Implementation() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
	TSubclassOf<AConsciousPawn> ConsciousPawnClassToSpawn;

protected:
	virtual bool InternalIsReachable_Implementation() override;
	
	virtual void InternalBeginExecute_Implementation() override;
	
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;
};
