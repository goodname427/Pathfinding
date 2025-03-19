// Fill out your copyright notice in the Description page of Project Settings.


#include "PeriodicProgressCommandComponent.h"

#include "ConsciousPawn.h"

int32 UPeriodicProgressCommandComponent::StaticCommandChannel = 2;

UPeriodicProgressCommandComponent::UPeriodicProgressCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Data.bNeedToTarget = false;
	Data.Channel = StaticCommandChannel;
	Data.bHiddenInCommandListMenu = true;
}


void UPeriodicProgressCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	OnCommandEnd.AddDynamic(this, &UPeriodicProgressCommandComponent::OnSelfCommandEnd);

	GetExecutePawn()->Receive(FTargetRequest(this));
}

void UPeriodicProgressCommandComponent::OnSelfCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	GetExecutePawn()->Receive(FTargetRequest(this));
}
