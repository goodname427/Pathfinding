// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command/CommandComponent.h"
#include "Battle/BattlePlayerState.h"
#include "Resource/ResourcePawn.h"
#include "CollectCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UCollectCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCollectCommandComponent();

public:
	DECLARE_COMMAND_NAME()

	bool CanCollect(const AResourcePawn* ResourcePawn) const;
	
protected:
	virtual bool InternalIsCommandEnable_Implementation(FString& OutDisableReason) const override;
	
	virtual bool InternalIsArgumentsValid_Implementation() const override;
	
	virtual void InternalBeginExecute_Implementation() override;

	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;

	virtual void InternalExecute_Implementation(float DeltaTime) override;

protected:
	UPROPERTY(Category = "Command|Collect", EditAnywhere)
	TSet<EResourceType> ResourceTypesToAllowCollecting;
};
