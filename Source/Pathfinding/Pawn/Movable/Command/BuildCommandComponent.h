// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/BuildingFramePawn.h"
#include "Building/BuildingPawn.h"
#include "Command/CommandComponent.h"
#include "BuildCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UBuildCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UBuildCommandComponent();

	virtual void BeginPlay() override;

public:
	DECLARE_COMMAND_NAME()

	virtual FName GetCommandName_Implementation() const override;

	virtual FString GetCommandDescription_Implementation() const override;

	virtual UObject* GetCommandIcon_Implementation() const override;

	virtual float GetRequiredTargetRadius_Implementation() const override;

protected:
	virtual bool InternalIsCommandEnable_Implementation() const override;

	virtual bool InternalIsArgumentsValid_Implementation() const override;

	bool IsValidLocationToBuild(const FVector& Location) const;

	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	virtual void InternalPushedToQueue_Implementation() override;
	
	virtual void InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason) override;


protected:
	virtual void InternalBeginTarget_Implementation() override;
	
	virtual void InternalEndTarget_Implementation(bool bCanceled) override;

	virtual void InternalTarget_Implementation(float DeltaTime) override;

private:
	void SpawnFrameActor();
	
	void TryDestroyFrameActor();
	
	UPROPERTY()
	ABuildingFramePawn* FrameActor;

	FBox BoundsToBuild;

protected:
	const ABuildingPawn* GetDefaultObjectToBuild() const;

protected:
	UPROPERTY(Category = "Command|Spawn", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABuildingPawn> PawnClassToBuild;
};
