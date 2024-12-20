// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "MoveCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UMoveCommandComponent final : public UCommandComponent
{
	GENERATED_BODY()

public:
	UMoveCommandComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	IMPL_GET_COMMAND_NAME()
	
	virtual float GetRequiredTargetRadius_Implementation() const override { return -1; }

	// Set args for move command, return true whether it's reachable
	bool SetMoveCommandArgs(UCommandComponent* InCommandNeedToMove, const FTargetRequest& InRequest);
	
protected:
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	UFUNCTION()
	void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	UPROPERTY()
	UCommandComponent* CommandNeedToMove;
};
