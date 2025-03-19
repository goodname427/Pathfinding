// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProgressCommandComponent.h"
#include "PeriodicProgressCommandComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UPeriodicProgressCommandComponent : public UProgressCommandComponent
{
	GENERATED_BODY()

public:
	UPeriodicProgressCommandComponent();
	
protected:
	virtual void BeginPlay() override;

public:
	DECLARE_COMMAND_CHANNEL()

protected:
	UFUNCTION()
	void OnSelfCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result);
	
};
