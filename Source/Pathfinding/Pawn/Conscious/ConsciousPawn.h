// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "ConsciousAIController.h"
#include "PFPawn.h"
#include "Command/MoveCommandComponent.h"
#include "ConsciousPawn.generated.h"

struct FTargetRequest;
class UCommandComponent;
class UConsciousPawnMovementComponent;

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
	void Receive(const FTargetRequest& Request);

protected:
	void ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request);

	UFUNCTION(BlueprintNativeEvent)
	UCommandComponent* ResolveRequestWithoutName(const FTargetRequest& Request);

	UFUNCTION(BlueprintCallable)
	UMoveCommandComponent* GetMoveCommand() const;

public:
	UFUNCTION(BlueprintCallable)
	AConsciousAIController* GetConsciousAIController() const { return ConsciousAIController; } 
	
protected:
	UPROPERTY()
	AConsciousAIController* ConsciousAIController;
	
	UPROPERTY(Transient, Category = "Command", VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, UCommandComponent*> Commands;
};
