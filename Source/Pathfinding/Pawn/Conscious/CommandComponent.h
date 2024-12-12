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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandBeginSignature, UCommandComponent*, Command);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommandEndSignature, UCommandComponent*, Command);

/**
 * 
 */
UCLASS(Blueprintable)
class PATHFINDING_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	FName GetCommandName() const;

	UFUNCTION(BlueprintCallable)
	AConsciousPawn* GetExecutePawn() const;

	UFUNCTION(BlueprintCallable)
	void SetCommandArgs(const FTargetRequest& InRequest);

	UFUNCTION(BlueprintCallable)
	bool CanExecute();

	UFUNCTION(BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintCallable)
	void EndExecute(bool bInSucceeded);
	
	UFUNCTION(BlueprintCallable)
	bool IsExecuting() const { return bExecuting; }

	UFUNCTION(BlueprintCallable)
	bool IsSucceeded() const { return bSucceeded;}

protected:
	UFUNCTION(BlueprintNativeEvent)
	bool InternalCanExecute();

	UFUNCTION(BlueprintNativeEvent)
	void InternalBeginExecute();

	UFUNCTION(BlueprintNativeEvent)
	void InternalEndExecute();
	
public:
	UPROPERTY(BlueprintAssignable)
	FCommandBeginSignature OnCommandBegin;
	
	UPROPERTY(BlueprintAssignable)
	FCommandEndSignature OnCommandEnd;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTargetRequest Request;
	
	bool bExecuting = false;
	bool bSucceeded = false;
};
