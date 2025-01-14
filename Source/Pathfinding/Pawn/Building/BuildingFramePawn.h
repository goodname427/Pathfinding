// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingPawn.h"
#include "BuildingFramePawn.generated.h"

UCLASS()
class PATHFINDING_API ABuildingFramePawn : public AConsciousPawn
{
	GENERATED_BODY()

public:
	ABuildingFramePawn();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
public:
	virtual void SetColor(const FLinearColor& InColor) override;;

	virtual void OnTarget(class ACommanderPawn* TargetCommander) override;;

public:
	void SetBuildingClassToBuild(TSubclassOf<ABuildingPawn> InBuildingClassToBuild, ACommanderPawn* InCommander);

	TSubclassOf<ABuildingPawn> GetBuildingClassToBuild() const { return BuildingClassToBuild; };

	ABuildingPawn* GetDefaultObjectToBuild() const { return BuildingClassToBuild.Get() ? BuildingClassToBuild->GetDefaultObject<ABuildingPawn>() : nullptr; }

	ACommanderPawn* GetCommander() const { return Commander; }

protected:
	UPROPERTY(Category = "BuildingFrame", VisibleAnywhere)
	UBuildingCommandComponent* BuildingCommandComponent;

	UPROPERTY(Category = "BuildingFrame", VisibleAnywhere, ReplicatedUsing="OnRep_BuildingClassToBuild")
	TSubclassOf<ABuildingPawn> BuildingClassToBuild;
	UFUNCTION()
	void OnRep_BuildingClassToBuild();

	UPROPERTY(Category = "BuildingFrame", VisibleAnywhere, Replicated)
	ACommanderPawn* Commander;
};
