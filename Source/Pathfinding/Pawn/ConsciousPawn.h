// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Controller/ConsciousAIController.h"
#include "Command/MoveCommandComponent.h"
#include "PFPawn.h"
#include "Command/MoveCommandComponent.h"
#include "ConsciousPawn.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewCommandChannelCreatedSignature, AConsciousPawn*, ConsciousPawn, UCommandChannel*, NewCommandChannel);

UCLASS()
class PATHFINDING_API AConsciousPawn : public APFPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AConsciousPawn();

protected:
	virtual void BeginPlay() override;

public:
	// Request Command
	UFUNCTION(BlueprintCallable)
	void Receive(const FTargetRequest& Request);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnReceive(const FTargetRequest& Request);

	virtual void ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request);

	UCommandComponent* ResolveRequestCommand(const FTargetRequest& Request);

	UFUNCTION(BlueprintNativeEvent)
	UCommandComponent* ResolveRequestWithoutName(const FTargetRequest& Request);

public:
	// Command
	UFUNCTION(BlueprintCallable)
	bool HasProgressCommand() const { return GetProgressChannel() != nullptr; }

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
	TMultiMap<FName, UCommandComponent*> Commands;

public:
	// Command Channel
	UPROPERTY(BlueprintAssignable)
	FNewCommandChannelCreatedSignature OnNewCommandChannelCreated;
	
	UFUNCTION(BlueprintCallable)
	UCommandComponent* GetCurrentCommand(int32 ChannelId) const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetCommandsInQueue(int32 ChannelId) const;

	UFUNCTION(BlueprintCallable)
	const UCommandChannel* GetProgressChannel() const { return GetCommandChannel(UProgressCommandComponent::StaticCommandChannel); }
	
	UFUNCTION(BlueprintCallable)
	UProgressCommandComponent* GetCurrentProgressCommand() const
	{
		return Cast<UProgressCommandComponent>(GetCurrentCommand(UProgressCommandComponent::StaticCommandChannel));
	}

	UFUNCTION(BlueprintCallable)
	const TArray<UProgressCommandComponent*>& GetProgressCommandsInQueue() const;
	
protected:
	UCommandChannel* GetCommandChannel(int32 ChannelId) const;
	
	UCommandChannel* GetOrCreateCommandChannel(int32 ChannelId);

	UPROPERTY()
	TMap<int32, UCommandChannel*> CommandChannels;

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
