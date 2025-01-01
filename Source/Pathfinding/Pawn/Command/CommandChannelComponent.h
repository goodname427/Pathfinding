// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
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
	{
		Command = InCommand;
		Request = InCommand->GetRequest();
	}

	UCommandComponent* Get() const
	{
		Command->SetCommandArgs(Request);
		return Command;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandChannelUpdatedSignature, UCommandChannelComponent*, CommandChannel);

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

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	int32 GetChannelId() const { return ChannelId; }

	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand() const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue() const;

	UFUNCTION(BlueprintCallable)
	const FTargetRequest& GetRequestAt(int32 Index) const { return CommandQueue[Index].Request; }

	void PushCommand(UCommandComponent* CommandToPush);

	void PopCommand(int32 CommandIndexToPop);

	void ClearCommands();

	void ExecuteNextCommand();

	void AbortCurrentCommand();

protected:
	AConsciousPawn* GetConsciousPawnOwner() const;
	
	// Server only
	UFUNCTION()
	void OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result);

public:
	UPROPERTY(BlueprintAssignable)
	FCommandChannelUpdatedSignature OnCommandUpdated;

private:
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
