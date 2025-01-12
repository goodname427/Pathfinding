// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "UObject/Object.h"
#include "CommandChannelComponent.generated.h"

class AConsciousPawn;

USTRUCT()
struct FCommandWrapper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UCommandComponent* Command;

	UPROPERTY()
	FTargetRequest Request;

	FCommandWrapper()
		: Command(nullptr)
	{
	}

	FCommandWrapper(UCommandComponent* InCommand)
		: Command(InCommand)
	{
		Request = InCommand->GetRequest();
	}

	UCommandComponent* Get() const
	{
		if (Command)
		{
			Command->SetCommandArgumentsSkipCheck(Request);
		}
		return Command;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandChannelUpdatedSignature, UCommandChannelComponent*, CommandChannel);

#define CLEAR_COMMAND_CHANNEL(CommandChannel, Reason)\
{ \
	if (CommandChannel->BeginClear()) \
	{ \
		CommandChannel->ClearCommands(Reason); \
		CommandChannel->EndClear(); \
	} \
}

#define DispatchCommand(FunctionName, ...) \
{ \
	if (AConsciousPawn* ConsciousPawn = GetConsciousPawnOwner()) \
	{ \
		ConsciousPawn->DispatchCommand_##FunctionName(##__VA_ARGS__); \
	} \
}
/**
 * 
 */
UCLASS(BlueprintType)
class PATHFINDING_API UCommandChannelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandChannelComponent();

	static UCommandChannelComponent* NewCommandChannel(AConsciousPawn* Owner, int32 InChannelId);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	int32 GetChannelId() const { return ChannelId; }

	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand() const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue() const;

	UFUNCTION(BlueprintCallable)
	const FTargetRequest& GetRequestAt(int32 Index) const { return CommandQueue[Index].Request; }

public:
	void PushCommand(UCommandComponent* CommandToPush);

	void PopCommand(int32 CommandIndexToPop);

	bool BeginClear();

	void EndClear();

	void ClearCommands(ECommandPoppedReason Reason);

	void ExecuteNextCommand();

protected:
	bool AbortCurrentCommand();

	AConsciousPawn* GetConsciousPawnOwner() const;

	// Server only
	UFUNCTION()
	void OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result);

public:
	UPROPERTY(BlueprintAssignable)
	FCommandChannelUpdatedSignature OnCommandUpdated;

private:
	bool bIsClearing = false;
	UPROPERTY(Transient)
	TArray<FCommandWrapper> PendingCommandQueue;

	UPROPERTY(ReplicatedUsing=OnRep_ChannelId)
	int32 ChannelId;
	UFUNCTION()
	void OnRep_ChannelId();

	UPROPERTY(Transient, ReplicatedUsing=OnRep_CommandQueue)
	TArray<FCommandWrapper> CommandQueue;
	UFUNCTION()
	void OnRep_CommandQueue() { if (OnCommandUpdated.IsBound()) OnCommandUpdated.Broadcast(this); }

	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentCommand)
	UCommandComponent* CurrentCommand;
	UFUNCTION()
	void OnRep_CurrentCommand() { if (OnCommandUpdated.IsBound()) OnCommandUpdated.Broadcast(this); }
};
