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

	virtual FString GetCommandDisplayName_Implementation() const override;

	virtual FString GetCommandDescription_Implementation() const override;

	virtual UObject* GetCommandIcon_Implementation() const override;

	virtual float GetProgressDuration_Implementation() const override;

	UFUNCTION(BlueprintCallable)
	void SetGatherLocation(const FVector& InSpawnLocation) { GatherLocation = InSpawnLocation; }

	UFUNCTION(BlueprintCallable)
	FVector GetGatherLocation() const { return GatherLocation; }

protected:
	virtual bool InternalIsArgumentsValid_Implementation() override;

	virtual void InternalPushedToQueue_Implementation() override;

	virtual void InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason) override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

protected:
	const AConsciousPawn* GetDefaultObjectToSpawn() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
	TSubclassOf<AConsciousPawn> ConsciousPawnClassToSpawn;

	FVector GatherLocation;
};
