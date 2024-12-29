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
	void Receive(const FTargetRequest& Request, bool bStartNewCommandQueue = true);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnReceive(const FTargetRequest& Request, bool bStartNewCommandQueue = true);
	
	virtual void ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request);

	UCommandComponent* ResolveRequestCommand(const FTargetRequest& Request);
	
	UFUNCTION(BlueprintNativeEvent)
	UCommandComponent* ResolveRequestWithoutName(const FTargetRequest& Request);
	
public:
	UFUNCTION(BlueprintCallable)
	AConsciousAIController* GetConsciousAIController() const { return ConsciousAIController; } 

	UFUNCTION(BlueprintCallable)
	virtual UMoveCommandComponent* GetMoveCommandComponent() const;

	UFUNCTION(BlueprintCallable)
	UMoveCommandComponent* GetCommandComponent(FName CommandName) const;

	UFUNCTION(BlueprintCallable)
	const TArray<UCommandComponent*>& GetAllCommands() const;

	UFUNCTION(BlueprintCallable)
	const UCommandComponent* AddCommand(TSubclassOf<UCommandComponent> CommandClassToAdd);
	
protected:
	UPROPERTY(Transient)
	AConsciousAIController* ConsciousAIController;
	
	TMultiMap<FName, UCommandComponent*> Commands;

public:
	UFUNCTION(BlueprintCallable)
	const FConsciousData& GetConsciousData() const { return ConsciousData; }
	
protected:
	UPROPERTY(Category = "State|Conscious", EditDefaultsOnly, BlueprintReadOnly)
	FConsciousData ConsciousData;
};
