// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual FString GetCommandDisplayName_Implementation() const override;

	virtual FString GetCommandDescription_Implementation() const override;

	virtual UObject* GetCommandIcon_Implementation() const override;

protected:
	virtual bool InternalIsCommandEnable_Implementation() const override;
	
	virtual void InternalBeginExecute_Implementation() override;
	
protected:
	const AConsciousPawn* GetDefaultObjectToBuild() const;

protected:
	UPROPERTY(Category = "Command|Spawn", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABuildingPawn> PawnClassToBuild;
};
