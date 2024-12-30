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

UCLASS()
class PATHFINDING_API AConsciousPawn : public APFPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AConsciousPawn();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
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
	UFUNCTION(BlueprintCallable)
	AConsciousAIController* GetConsciousAIController() const { return ConsciousAIController; }

	UFUNCTION(BlueprintCallable)
	bool HasProgressCommand() const { return bHasProgressCommand; }

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
	UPROPERTY(Transient)
	AConsciousAIController* ConsciousAIController;

	TMultiMap<FName, UCommandComponent*> Commands;

	bool bHasProgressCommand;

public:
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
