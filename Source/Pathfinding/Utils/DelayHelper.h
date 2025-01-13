// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/Object.h"
#include "DelayHelper.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UDelayHelper : public UObject
{
	GENERATED_BODY()

public:
	static void Delay(UObject* WorldContextObject, float Duration, TFunction<void()> InFuncToDelayExecute);

private:
	UFUNCTION()
	void OnDelayFinished();

	TFunction<void()> FuncToDelayExecute;
};
