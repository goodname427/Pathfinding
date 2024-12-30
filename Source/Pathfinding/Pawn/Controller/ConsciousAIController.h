// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Command/CommandComponent.h"
#include "Command/ProgressCommandComponent.h"
#include "ConsciousAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandUpdatedSignature, AConsciousAIController*, AIController);

UCLASS()
class PATHFINDING_API AConsciousAIController : public AAIController
{
	GENERATED_BODY()

public:
	AConsciousAIController();

public:
	UPROPERTY(BlueprintAssignable)
	FCommandUpdatedSignature OnCommandUpdated;
	
	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand() const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue() const { return CommandQueue; }

	UFUNCTION(BlueprintCallable)
	const FTargetRequest& GetRequestAt(int32 Index) const { return RequestQueue[Index];}

	UFUNCTION(BlueprintCallable)
	UProgressCommandComponent* GetCurrentProgressCommand() const {return Cast<UProgressCommandComponent>(GetCurrentCommand()); }

	UFUNCTION(BlueprintCallable)
	const TArray<UProgressCommandComponent*>& GetProgressCommandsInQueue() const;

	void PushCommand(UCommandComponent* CommandToPush);
	void PopCommand(const FTargetRequest& PopRequest);
	void ClearCommands();

	void ExecuteNextCommand();
	void AbortCurrentCommand();

protected:
	void ExecuteCommand(UCommandComponent* Command);

	UFUNCTION()
	void OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result);

private:
	UPROPERTY(Transient)
	TArray<UCommandComponent*> CommandQueue;

	TArray<FTargetRequest> RequestQueue;

	UPROPERTY(Transient)
	UCommandComponent* CurrentCommand;

public:
	static FName CurrentCommandKeyName;
};
