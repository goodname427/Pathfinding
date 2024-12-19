// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conscious/CommandComponent.h"
#include "Battle/BattlePlayerState.h"
#include "Resource/ResourcePawn.h"
#include "CollectCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UCollectCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCollectCommandComponent();

public:
	IMPL_GET_COMMAND_NAME()

	bool CanCollect(const AResourcePawn* ResourcePawn) const;
	
protected:
	virtual bool InternalIsReachable_Implementation() override;
	
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

protected:
	UPROPERTY(Category = "Collect", EditAnywhere)
	TSet<EResourceType> ResourceTypesToAllowCollecting;
	
public:
	static FName CommandName;
};
