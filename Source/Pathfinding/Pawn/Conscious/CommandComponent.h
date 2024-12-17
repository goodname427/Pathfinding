// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CommandComponent.generated.h"

class APFPawn;
class AConsciousPawn;

USTRUCT(BlueprintType)
struct FTargetRequest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CommandName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APFPawn* TargetPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;
};

UENUM(BlueprintType)
enum class ECommandExecuteResult : uint8
{
	Success,
	Failed,
	Aborted,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandBeginSignature, UCommandComponent*, Command);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommandEndSignature, UCommandComponent*, Command, ECommandExecuteResult, Result);

/**
 * 
 */
UCLASS(Blueprintable)
class PATHFINDING_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Command Default Arguments
	UFUNCTION(BlueprintNativeEvent)
	FName GetCommandName() const;

	UFUNCTION(BlueprintNativeEvent)
	float GetRequiredTargetRadius();

public:
	// State Query
	UFUNCTION(BlueprintCallable)
	AConsciousPawn* GetExecutePawn() const;

	UFUNCTION(BlueprintCallable)
	bool IsExecuting() const { return bExecuting; }

public:
	// Execute
	UFUNCTION(BlueprintCallable)
	void SetCommandArgs(const FTargetRequest& InRequest);

	UFUNCTION(BlueprintCallable)
	bool CanExecute();

	UFUNCTION(BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintCallable)
	void EndExecute(ECommandExecuteResult Result);

protected:
	// Internal Implementation
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTargetRequest Request;

	bool bExecuting = false;
};
