// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPawn.h"
#include "UObject/Object.h"
#include "CommandComponent.generated.h"

class UCommandComponent;
class APFPawn;
class AConsciousPawn;
class AConsciousAIController;

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CommandName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bNeedToTarget"))
	float RequiredTargetRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bNeedToTarget;

	FCommandData()
	{
		CommandName = "";
		RequiredTargetRadius = 250.f;
		bNeedToTarget = true;
	}

	float GetRequiredTargetRadius() const  { return bNeedToTarget? RequiredTargetRadius : -1; }
};

USTRUCT(BlueprintType)
struct FTargetRequest
{
	GENERATED_USTRUCT_BODY()

	FTargetRequest(): TargetPawn(nullptr), Command(nullptr)
	{
	}

	explicit FTargetRequest(FName InCommandName) : CommandName(InCommandName), TargetPawn(nullptr), Command(nullptr)
	{
	}

	explicit FTargetRequest(APFPawn* InTargetPawn): TargetPawn(InTargetPawn), Command(nullptr)
	{
	}

	explicit FTargetRequest(const FVector& InTargetLocation): TargetPawn(nullptr), TargetLocation(InTargetLocation),
	                                                          Command(nullptr)
	{
	}

	explicit FTargetRequest(UCommandComponent* InCommand);

	FTargetRequest(FName InCommandName, APFPawn* InTargetPawn): CommandName(InCommandName), TargetPawn(InTargetPawn),
	                                                            Command(nullptr)
	{
	}

	FTargetRequest(FName InCommandName, const FVector& InTargetLocation): CommandName(InCommandName),
	                                                                      TargetPawn(nullptr),
	                                                                      TargetLocation(InTargetLocation),
	                                                                      Command(nullptr)
	{
	}

	FTargetRequest(FName InCommandName, APFPawn* InTargetPawn, const FVector& InTargetLocation):
		CommandName(InCommandName), TargetPawn(InTargetPawn), TargetLocation(InTargetLocation), Command(nullptr)
	{
	}

	template<class TCommand>
	static FTargetRequest Make()
	{
		return FTargetRequest(TCommand::StaticCommandName);
	}

	template<class TCommand>
	static FTargetRequest Make(APFPawn* InTargetPawn)
	{
		return FTargetRequest(TCommand::StaticCommandName, InTargetPawn);
	}

	template<class TCommand>
	static FTargetRequest Make(const FVector InTargetLocation)
	{
		return FTargetRequest(TCommand::StaticCommandName, InTargetLocation);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CommandName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APFPawn* TargetPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	UCommandComponent* Command;

	FVector GetTargetLocation() const { return TargetPawn ? TargetPawn->GetActorLocation() : TargetLocation; }

	APFPawn* GetTargetPawn() const { return TargetPawn; }

	template <class T>
	T* GetTargetPawn() const { return Cast<T>(TargetPawn); }
};

UENUM(BlueprintType)
enum class ECommandExecuteResult : uint8
{
	Success,
	Failed,
	Aborted,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandBeginSignature, UCommandComponent*, Command);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommandEndSignature, UCommandComponent*, Command, ECommandExecuteResult,
                                             Result);

#define DECLARE_COMMAND_NAME() \
static FName StaticCommandName;

/**
 * Command component that can only be attached to ConsciousPawn
 */
UCLASS(Abstract, Blueprintable, ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandComponent();

public:
	// Command Default Arguments
	FName GetCommandName() const { return Data.CommandName; };

	UFUNCTION(BlueprintNativeEvent)
	float GetRequiredTargetRadius() const;

	bool IsNeedToTarget() const { return Data.bNeedToTarget; };

protected:
	UPROPERTY(Category = "Command", EditDefaultsOnly, BlueprintReadOnly)
	FCommandData Data;

public:
	// State Query
	UFUNCTION(BlueprintCallable)
	AConsciousPawn* GetExecutePawn() const;

	template <class T>
	T* GetExecutePawn() const { return Cast<T>(GetOwner()); }

	UFUNCTION(BlueprintCallable)
	AConsciousAIController* GetExecuteController() const;

	UFUNCTION(BlueprintCallable)
	bool IsExecuting() const { return bExecuting; }

	UFUNCTION(BlueprintCallable)
	const FTargetRequest& GetRequest() const { return Request; };

public:
	// Execute
	UFUNCTION(BlueprintCallable)
	bool SetCommandArgs(const FTargetRequest& InRequest);

	// Check whether the command args is reachable, it will be check in advance
	UFUNCTION(BlueprintCallable)
	bool IsReachable();

	UFUNCTION(BlueprintCallable)
	bool IsTargetInRequiredRadius() const;

	// Check whether the current status can be executed
	UFUNCTION(BlueprintCallable)
	bool CanExecute();

	UFUNCTION(BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintCallable)
	void EndExecute(ECommandExecuteResult Result);

protected:
	// Internal Implementation
	UFUNCTION(BlueprintNativeEvent)
	bool InternalIsReachable();

	UFUNCTION(BlueprintNativeEvent)
	bool InternalCanExecute();

	UFUNCTION(BlueprintNativeEvent)
	void InternalBeginExecute();

	UFUNCTION(BlueprintNativeEvent)
	void InternalEndExecute(ECommandExecuteResult Result);

public:
	UPROPERTY(BlueprintAssignable)
	FCommandBeginSignature OnCommandBegin;

	UPROPERTY(BlueprintAssignable)
	FCommandEndSignature OnCommandEnd;

protected:
	FTargetRequest Request;

	bool bExecuting = false;
};
