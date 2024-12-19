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
	UMoveCommandComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	IMPL_GET_COMMAND_NAME()
	
	virtual float GetRequiredTargetRadius_Implementation() const override { return -1; }

	void SetMoveCommandArgs(UCommandComponent* InCommandNeedMove);
	
protected:
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	UFUNCTION()
	void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	UPROPERTY()
	UCommandComponent* CommandNeedMove;
	
public:
	static FName CommandName;
};
