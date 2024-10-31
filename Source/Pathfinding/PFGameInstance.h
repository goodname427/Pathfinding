// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "PFGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSubclassOf<UUserWidget>> WidgetSettings;
};
