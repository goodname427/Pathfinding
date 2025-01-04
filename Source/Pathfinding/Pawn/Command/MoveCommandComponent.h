// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "MoveCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UMoveCommandComponent final : public UCommandComponent
{
	GENERATED_BODY()

public:
	UMoveCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	virtual float GetRequiredTargetRadius_Implementation() const override { return -1; }

	// Set args for move command, return true whether it's reachable
	bool SetMoveCommandArguments(UCommandComponent* InCommandNeedToMove, const FTargetRequest& InRequest);

protected:
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	virtual void InternalExecute_Implementation(float DeltaTime) override;

	// Server only
	UFUNCTION()
	void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	UPROPERTY()
	UCommandComponent* CommandNeedToMove;

protected:
	virtual void InternalBeginTarget_Implementation() override;
	
	virtual void InternalEndTarget_Implementation() override;

	virtual void InternalTarget_Implementation(float DeltaTime) override;

private:
	UPROPERTY()
	AActor* FlagActor;

	UPROPERTY(Category = "Command|Move", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> FlagActorClass;
};
