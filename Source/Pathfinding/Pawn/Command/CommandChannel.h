// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "ProgressCommandComponent.h"
#include "UObject/Object.h"
#include "CommandChannel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandChannelUpdatedSignature, UCommandChannel*, CommandChannel);

/**
 * 
 */
UCLASS(BlueprintType)
class PATHFINDING_API UCommandChannel : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCommandChannelUpdatedSignature OnCommandUpdated;
	
	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand() const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue() const { return CommandQueue; }

	UFUNCTION(BlueprintCallable)
	const FTargetRequest& GetRequestAt(int32 Index) const { return RequestQueue[Index];}

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
};
