// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManage/PFGameInstance.h"
#include "PFUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "WidgetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPFGameInstance* GameInstance;

protected:
	// Try Get Widget Of The Specified Name
	UUserWidget* TryGetWidget(const FString& InWidgetName);
	// Show The Specified Widget And Hide The Current Widget
	void Show(UUserWidget* InTargetWidget);

public:
	// Show Widget Of The Specified Name And Hide The Current Widget
	void Show(const FString& InWidgetName);
	// Clear Current Widget
	void Clear();
	// Show Error Dialog
	void ShowErrorDialog(const FString& ErrorText);

private:
	UUserWidget* CurrentWidget;
	// The Cache of All Loaded Widgets
	TMap<const FString, UUserWidget*> LoadedWidgets;
};
