// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conscious/CommandComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "MoveCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UMoveCommandComponent final : public UCommandComponent
{
	GENERATED_BODY()

public:
	virtual FName GetCommandName_Implementation() const override { return MoveCommandName; }
	virtual float GetRequiredTargetRadius_Implementation() override { return -1; }

	void SetMoveCommandArgs(float InAcceptanceRadius);
	
protected:
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	UFUNCTION()
	void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	float AcceptanceRadius;
	
public:
	static FName MoveCommandName;
};
