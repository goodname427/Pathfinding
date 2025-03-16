// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommandComponent.h"
#include "AttackCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UAttackCommandComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UAttackCommandComponent();

public:
	DECLARE_COMMAND_NAME()

protected:
	virtual bool InternalIsCommandEnable_Implementation(FString& OutDisableReason) const override;
	virtual bool InternalIsArgumentsValid_Implementation() const override;

	virtual void InternalBeginExecute_Implementation() override;
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;
	virtual void InternalExecute_Implementation(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyDamageToTargetPawn();
};
