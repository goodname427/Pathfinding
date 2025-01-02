// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Controller/ConsciousAIController.h"
#include "Command/MoveCommandComponent.h"
#include "PFPawn.h"
#include "Command/MoveCommandComponent.h"
#include "ConsciousPawn.generated.h"

class UCommandChannelComponent;
struct FTargetRequest;
class UCommandComponent;
class UConsciousPawnMovementComponent;

USTRUCT(BlueprintType)
struct FConsciousData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EResourceType, int32> ResourcesToAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnDuration;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandChannelCreatedSignature, AConsciousPawn*, ConsciousPawn);

#define SEND_TO_SELF_AUTHORITY(Request)\
	if (HasAuthority())\
	{\
		Receive((Request));\
	}

UCLASS()
class PATHFINDING_API AConsciousPawn : public APFPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AConsciousPawn();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

	virtual void PostInitializeComponents() override;

public:
	// Request Command
	// Server only
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Receive(const FTargetRequest& Request);

protected:
	// Server only
	UFUNCTION(BlueprintNativeEvent)
	void OnReceive(const FTargetRequest& Request);

	// Server only
	virtual void ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request);

	// Server only
	UCommandComponent* ResolveRequestCommand(const FTargetRequest& Request);

	// Server only
	UFUNCTION(BlueprintNativeEvent)
	UCommandComponent* ResolveRequestWithoutName(const FTargetRequest& Request);

public:
	// Command
	UFUNCTION(BlueprintCallable)
	virtual UMoveCommandComponent* GetMoveCommandComponent() const;

	template <class TCommand>
	TCommand* GetCommandByName() const;

	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCommandByName(FName CommandName) const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsByName(FName CommandName) const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetAllCommands() const;

	UFUNCTION(BlueprintCallable)
	const UCommandComponent* AddCommand(TSubclassOf<UCommandComponent> CommandClassToAdd);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void DispatchCommand_BeginExecute(UCommandComponent* Command, const FTargetRequest& Request);

	UFUNCTION(NetMulticast, Reliable)
	void DispatchCommand_EndExecute(UCommandComponent* Command, ECommandExecuteResult Result);

	UFUNCTION(NetMulticast, Reliable)
	void DispatchCommand_OnPushedToQueue(UCommandComponent* Command);

	UFUNCTION(NetMulticast, Reliable)
	void DispatchCommand_OnPoppedFromQueue(UCommandComponent* Command, ECommandPoppedReason Reason);
	
protected:
	TMultiMap<FName, UCommandComponent*> CommandList;

public:
	// Command Channel
	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand(int32 ChannelId) const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue(int32 ChannelId) const;

	UFUNCTION(BlueprintCallable)
	const UCommandChannelComponent* GetProgressChannel() const { return GetCommandChannel(UProgressCommandComponent::StaticCommandChannel); }
	
	UFUNCTION(BlueprintCallable)
	UProgressCommandComponent* GetCurrentProgressCommand() const
	{
		return Cast<UProgressCommandComponent>(GetCurrentCommand(UProgressCommandComponent::StaticCommandChannel));
	}

	UFUNCTION(BlueprintCallable)
	const TArray<UProgressCommandComponent*>& GetProgressCommandsInQueue() const;

public:
	UPROPERTY(BlueprintAssignable)
	FCommandChannelCreatedSignature OnCommandChannelCreated;
	
protected:
	UCommandChannelComponent* GetCommandChannel(int32 ChannelId) const;

	// Server only
	UCommandChannelComponent* GetOrCreateCommandChannel(int32 ChannelId);

	// Client only
	void AddCommandChannel(UCommandChannelComponent* CommandChannel);
	
	friend class UCommandChannelComponent;
	TMap<int32, UCommandChannelComponent*> CommandChannelMap;
	
public:
	// Conscious Data
	UFUNCTION(BlueprintCallable)
	const FConsciousData& GetConsciousData() const { return ConsciousData; }

protected:
	UPROPERTY(Category = "State|Conscious", EditDefaultsOnly, BlueprintReadOnly)
	FConsciousData ConsciousData;
};

template <class TCommand>
TCommand* AConsciousPawn::GetCommandByName() const
{
	return Cast<TCommand>(GetCommandByName(TCommand::StaticCommandName));
}
