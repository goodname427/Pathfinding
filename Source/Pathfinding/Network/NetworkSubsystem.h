// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkSubsystem.generated.h"

UENUM(BlueprintType)
enum class EGameNetworkState : uint8
{
	Startup UMETA(DisplayName = "Startup"),
	MainMenu UMETA(DisplayName = "MainMenu"),
	ServerList UMETA(DisplayName = "ServerList"),
	LoadingScreen UMETA(DisplayName = "LoadingScreen"),
	ErrorDialog UMETA(DisplayName = "ErrorDialog"),
	Playing UMETA(DisplayName = "Playing"),
	Unknown UMETA(DisplayName = "Unknown")
};

/**
 * 
 */
UCLASS()
class PATHFINDING_API UNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);

public:
	// State
	EGameNetworkState GetCurrentState() const { return CurrentState; }

	bool IsCurrentState(EGameNetworkState InState) const { return CurrentState == InState; }
	
	bool TransitionToState(EGameNetworkState InDesiredState);

private:
	EGameNetworkState CurrentState;

public:
	// Widget
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();

private:
	class UWidgetSubsystem* WidgetSubsystem;

public:
	// Network
	void DestorySession();
};
