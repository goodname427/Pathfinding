
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WidgetSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig)
class PATHFINDING_API UWidgetSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Default")
	TMap<FName, TSubclassOf<UUserWidget>> WidgetClasses;
};
