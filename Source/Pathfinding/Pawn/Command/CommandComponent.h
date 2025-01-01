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

constexpr int32 GCommandChannel_Default = 0;

UENUM()
enum class ETargetRequestType : uint8
{
	StartNew,
	Append,
	Clear,
	AbortOrPop
};

USTRUCT(BlueprintType)
struct FTargetRequest
{
	GENERATED_USTRUCT_BODY()

	FTargetRequest(): TargetPawn(nullptr), Command(nullptr)
	{
		Type = ETargetRequestType::StartNew;
		OverrideCommandChannel = -1;
		CommandIndexToPop = -1;
	}

	explicit FTargetRequest(FName InCommandName) : FTargetRequest()
	{
		CommandName = InCommandName; 
	}

	explicit FTargetRequest(APFPawn* InTargetPawn): FTargetRequest()
	{
		TargetPawn = InTargetPawn;
	}

	explicit FTargetRequest(const FVector& InTargetLocation): FTargetRequest()
	{
		TargetLocation = InTargetLocation;
	}

	explicit FTargetRequest(UCommandComponent* InCommand);

	FTargetRequest(UCommandComponent* InCommand, APFPawn* InTargetPawn): FTargetRequest(InCommand)
	{
		TargetPawn = InTargetPawn;
	}

	FTargetRequest(UCommandComponent* InCommand, const FVector& InTargetLocation): FTargetRequest(InCommand)
	{
		TargetLocation = InTargetLocation;
	}

	FTargetRequest(FName InCommandName, APFPawn* InTargetPawn): FTargetRequest(InCommandName)
	{
		TargetPawn = InTargetPawn;
	}

	FTargetRequest(FName InCommandName, const FVector& InTargetLocation): FTargetRequest(InCommandName)
	{
		TargetLocation = InTargetLocation;
	}

	FTargetRequest(FName InCommandName, APFPawn* InTargetPawn, const FVector& InTargetLocation): FTargetRequest(InCommandName)
	{
		TargetPawn = InTargetPawn;
		TargetLocation = InTargetLocation;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	ETargetRequestType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	int32 OverrideCommandChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta=(EditCondition = "Type == ETargetRequestType::AbortOrPop"))
	int32 CommandIndexToPop;

	FVector GetTargetLocation() const { return TargetPawn ? TargetPawn->GetActorLocation() : TargetLocation; }

	APFPawn* GetTargetPawn() const { return TargetPawn; }

	template <class T>
	T* GetTargetPawn() const { return Cast<T>(TargetPawn); }

	bool IsOverrideCommandChannel() const { return OverrideCommandChannel > GCommandChannel_Default; }
};

#define GET_COMMAND_CHANNEL(Request, Command) (Request.IsOverrideCommandChannel()? Request.OverrideCommandChannel : Command->GetCommandChannel())

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAbortCurrentCommand;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CommandChannel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowedClasses="Texture"))
	UObject* CommandIcon;

	FCommandData()
	{
		CommandName = NAME_None;
		RequiredTargetRadius = 250.f;
		bNeedToTarget = true;
		bAbortCurrentCommand = true;
		CommandChannel = GCommandChannel_Default;
		CommandIcon = nullptr;
	}

	float GetRequiredTargetRadius() const  { return bNeedToTarget? RequiredTargetRadius : -1; }
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

#define DECLARE_COMMAND_CHANNEL() \
static int32 StaticCommandChannel;

#define END_EXECUTE_AUTHORITY_FOR(Command, Result)\
	if (Command->GetOwnerRole() == ROLE_Authority) \
	{ \
		Command->EndExecute(Result); \
	}

#define END_EXECUTE_AUTHORITY(Result) END_EXECUTE_AUTHORITY_FOR(this, Result)

#define AUTHORITY_CHECK() if (GetOwnerRole() < ROLE_Authority) return

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetRequiredTargetRadius() const;

	bool IsNeedToTarget() const { return Data.bNeedToTarget; };

	bool IsAbortCurrentCommand() const { return Data.bAbortCurrentCommand; };

	int32 GetCommandChannel() const { return Data.CommandChannel; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UObject* GetCommandIcon() const;

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
	friend class ACommandChannel;
	
	// Called On Pushed To Command Queue
	void OnPushedToQueue();

	// Called On Popped From Command Queue, just called when command is aborted or popped directly
	void OnPoppedFromQueue();
	
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

	UFUNCTION(BlueprintNativeEvent)
	void InternalPushedToQueue();

	UFUNCTION(BlueprintNativeEvent)
	void InternalPoppedFromQueue();
	
public:
	UPROPERTY(BlueprintAssignable)
	FCommandBeginSignature OnCommandBegin;

	UPROPERTY(BlueprintAssignable)
	FCommandEndSignature OnCommandEnd;

protected:
	FTargetRequest Request;

	bool bExecuting = false;
};
