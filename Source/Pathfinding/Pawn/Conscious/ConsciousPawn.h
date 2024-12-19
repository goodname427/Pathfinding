// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conscious/CommandComponent.h"
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
	void Receive(const FTargetRequest& Request, bool bStartNewCommandQueue = true);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnReceive(const FTargetRequest& Request, bool bStartNewCommandQueue = true);
	
	void ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request);

	UFUNCTION(BlueprintNativeEvent)
	UCommandComponent* ResolveRequestWithoutName(const FTargetRequest& Request);
	
public:
	UFUNCTION(BlueprintCallable)
	AConsciousAIController* GetConsciousAIController() const { return ConsciousAIController; } 

	UFUNCTION(BlueprintCallable)
	virtual UMoveCommandComponent* GetMoveCommandComponent() const;
protected:
	UPROPERTY(Transient)
	AConsciousAIController* ConsciousAIController;
	
	UPROPERTY(Transient)
	TMap<FName, UCommandComponent*> Commands;
};
