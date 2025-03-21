﻿
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

public:
	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta=(AllowedClasses="SoundBase"))
	FSoftObjectPath ButtonClickSound;

	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta=(AllowedClasses="SoundBase"))
	FSoftObjectPath WidgetShowSound;

	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta=(AllowedClasses="SoundBase"))
	FSoftObjectPath WidgetHideSound;
};
