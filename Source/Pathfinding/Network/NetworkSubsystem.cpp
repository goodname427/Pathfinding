// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkSubsystem.h"
#include "WidgetSubsystem.h"
#include "PFUtils.h"
#include <Kismet/GameplayStatics.h>


void UNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CurrentState = EGameNetworkState::Startup;
	WidgetSubsystem = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
}

bool UNetworkSubsystem::TransitionToState(EGameNetworkState InDesiredState)
{
	NULL_CHECK_RET(WidgetSubsystem, false);

	if (IsCurrentState(InDesiredState))
	{
		WidgetSubsystem->ShowErrorDialog("Error: Can Not Transition To The Same State");
		return false;
	}

	if (IsCurrentState(EGameNetworkState::Playing))
	{
		DestorySession();
	}
	
	CurrentState = InDesiredState;

	return true;
}

void UNetworkSubsystem::ShowMainMenu()
{
	// NULL_CHECK(WidgetSubsystem);
	if (WidgetSubsystem == nullptr)
	{
		WidgetSubsystem = GetGameInstance()->GetSubsystem<UWidgetSubsystem>();
	}

	if (!IsCurrentState(EGameNetworkState::Startup))
	{
		return;
	}

	if (IsCurrentState(EGameNetworkState::Playing))
	{
		UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
	}

	if (TransitionToState(EGameNetworkState::MainMenu))
	{
		WidgetSubsystem->Show(TEXT("MainMenu"));
	}
}

void UNetworkSubsystem::DestorySession()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
}
