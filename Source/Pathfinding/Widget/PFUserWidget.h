// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PFUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void PreAddToViewport();

protected:
	UPROPERTY(EditDefaultsOnly)
	uint32 bSetInputModeUIOnly : 1;
};
