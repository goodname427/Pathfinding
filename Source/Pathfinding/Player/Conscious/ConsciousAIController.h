// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ConsciousAIController.generated.h"

UCLASS()
class PATHFINDING_API AConsciousAIController : public AAIController
{
	GENERATED_BODY()

public:
	AConsciousAIController();

protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;

protected:
	// AI
	UPROPERTY(Category = "AI", VisibleDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;
};
