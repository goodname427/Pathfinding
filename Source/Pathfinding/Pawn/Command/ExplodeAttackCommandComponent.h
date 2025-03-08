// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackCommandComponent.h"
#include "ExplodeAttackCommandComponent.generated.h"


UCLASS(ClassGroup=(Command), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UExplodeAttackCommandComponent : public UAttackCommandComponent
{
	GENERATED_BODY()

public:
	UExplodeAttackCommandComponent();

public:
	DECLARE_COMMAND_NAME()

protected:
	virtual bool InternalIsArgumentsValid_Implementation() const override;
	virtual void InternalBeginExecute_Implementation() override;
	virtual void InternalEndExecute_Implementation(ECommandExecuteResult Result) override;
	virtual void InternalExecute_Implementation(float DeltaTime) override;
	
	virtual void ApplyDamageToTargetPawn_Implementation() override;

protected:
	UPROPERTY(Category = "Command|Explode", EditAnywhere, BlueprintReadOnly)
	float ExplodeRadius;
};
