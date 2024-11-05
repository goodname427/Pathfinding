// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PFUserWidget.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPFUserWidget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PATHFINDING_API IPFUserWidget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAddToViewport();
	void OnAddToViewport_Implementation() {}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRemoveFromViewport();
	void OnRemoveFromViewport_Implementation() {}
};
