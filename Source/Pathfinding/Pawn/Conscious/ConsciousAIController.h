// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Conscious/CommandComponent.h"
#include "ConsciousAIController.generated.h"

UCLASS()
class PATHFINDING_API AConsciousAIController : public AAIController
{
	GENERATED_BODY()

public:
	AConsciousAIController();

public:
	UCommandComponent* GetCurrentCommand() const;

	void PushCommand(UCommandComponent* Command);
	void ClearAllCommands();

	void ExecuteCommandQueue();

protected:
	void ExecuteNextCommand();
	void ExecuteCommand(UCommandComponent* Command);
	void AbortCurrentCommand();

	UFUNCTION()
	void OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result);

private:
	TQueue<UCommandComponent*> CommandQueue;

	UPROPERTY()
	UCommandComponent* CurrentCommand;
	
public:
	static FName CurrentCommandKeyName;
};
