// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PFBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static const UPFGameSettings* GetPFGameSettings();

	// UFUNCTION(BlueprintCallable)
	// static UUserWidget* CreateAndAddWidgetTo(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, TArray<UUserWidget*>& WidgetArray, UPanelWidget* ParentPanelWidget);
};
