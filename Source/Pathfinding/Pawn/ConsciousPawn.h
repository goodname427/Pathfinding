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

UENUM(meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAllowedCreateMethod : uint32
{
	Spawn = 1,
	Build = 2,
};
ENUM_CLASS_FLAGS(EAllowedCreateMethod)
#define TO_FLAG(CreateMethod) 1 << (static_cast<int32>(CreateMethod) - 1)

USTRUCT(BlueprintType)
struct FConsciousData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Bitmask, BitmaskEnum = "EAllowedCreateMethod"))
	int32 AllowedCreateMethod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="AllowedCreateMethod != 0", EditConditionHides))
	TMap<EResourceType, int32> ResourceCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta = (ClampMin = 0, EditCondition = "AllowedCreateMethod != 0", EditConditionHides))
	float CreateDuration;

	bool IsAllowedToCreate() const { return AllowedCreateMethod != 0; }

	bool IsAllowedToCreate(EAllowedCreateMethod Method) const
	{
		return AllowedCreateMethod & TO_FLAG(Method);
	}

	bool IsAllowedToSpawn() const { return IsAllowedToCreate(EAllowedCreateMethod::Spawn); }
	bool IsAllowedToBuild() const { return IsAllowedToCreate(EAllowedCreateMethod::Build); }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandChannelCreatedSignature, AConsciousPawn*, ConsciousPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandListUpdatedSignature, AConsciousPawn*, ConsciousPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceivedRequestSignature, AConsciousPawn*, ConsciousPawn, const FTargetRequest&, Request);

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
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(BlueprintAssignable)
	FReceivedRequestSignature OnReceivedRequest;
	
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
	const TArray<UCommandComponent*>& GetAllCommandsForCommandListMenu() const;
	
	void RefreshCommandList();
	
	UFUNCTION(BlueprintCallable)
	UCommandComponent* AddNewCommand(TSubclassOf<UCommandComponent> CommandClassToAdd);

	template<class TCommand>
	TCommand* AddNewCommand() { return Cast<TCommand>(AddNewCommand(TCommand::StaticClass())); }
	
	void AddCommand(UCommandComponent* CommandToAdd);
	void RemoveCommand(UCommandComponent* CommandToRemove);

public:
	UPROPERTY(BlueprintAssignable)
	FCommandListUpdatedSignature OnCommandListUpdated;
	
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
	const UCommandChannelComponent* GetProgressChannel() const
	{
		return GetCommandChannel(UProgressCommandComponent::StaticCommandChannel);
	}

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
