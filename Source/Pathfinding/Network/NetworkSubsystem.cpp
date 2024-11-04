// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkSubsystem.h"
#include "WidgetSubsystem.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/OnlineSession.h"


void UNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UNetworkSubsystem::DestroySession()
{
	// todo
	// APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
}

void UNetworkSubsystem::HostRoom()
{
	// todo
}

void UNetworkSubsystem::JoinRoom()
{
	// todo
	UE_LOG_TEMP(TEXT("Join Room ____"));
}
