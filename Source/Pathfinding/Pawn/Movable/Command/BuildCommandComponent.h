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

	virtual float GetRequiredTargetRadius_Implementation() const override;

protected:
	virtual bool InternalIsCommandEnable_Implementation() const override;

	virtual bool InternalIsArgumentsValid_Implementation() const override;

	bool IsValidLocationToBuild(const AActor* Actor, const FVector& Location, bool bOnlyCollidingComponents) const;

	virtual void InternalBeginExecute_Implementation() override;

protected:
	virtual void InternalBeginTarget_Implementation() override;
	
	virtual void InternalEndTarget_Implementation() override;

	virtual void InternalTarget_Implementation(float DeltaTime) override;

private:
	UPROPERTY()
	AActor* FlagActor;

	FVector FlagMeshRelativeLocation;

	UPROPERTY()
	UStaticMeshComponent* FlagMesh;

protected:
	const ABuildingPawn* GetDefaultObjectToBuild() const;

protected:
	UPROPERTY(Category = "Command|Spawn", EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABuildingPawn> PawnClassToBuild;
};
