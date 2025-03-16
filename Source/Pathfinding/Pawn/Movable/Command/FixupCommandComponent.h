// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/BuildingPawn.h"
#include "Command/CommandComponent.h"
#include "FixupCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UFixupCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFixupCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	bool CanFixup(const ABuildingPawn* Building) const;

protected:
	virtual bool InternalIsCommandEnable_Implementation(FString& OutDisableReason) const override;

	virtual bool InternalIsArgumentsValid_Implementation() const override;

	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	virtual void InternalExecute_Implementation(float DeltaTime) override;

protected:
	UPROPERTY(Category = "Command|Fixup", EditAnywhere, BlueprintReadOnly)
	int32 PointPerFixup;

	UPROPERTY(Category = "Command|Fixup", EditAnywhere, BlueprintReadOnly)
	TMap<EResourceType, int32> ResourceCostPerFixup;
};
