// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CommandComponent.h"
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

public:
	UCommandComponent* GetCurrentCommand() const;
	void ExecuteCommand(UCommandComponent* Command);
	void CancelCommand();

	UFUNCTION()
	void OnCommandEnd(UCommandComponent* Command);

public:
	static FName CurrentCommandKeyName;
};
