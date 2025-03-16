// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressCommandComponent.h"

#include "PFUtils.h"
#include "Net/UnrealNetwork.h"

int32 UProgressCommandComponent::StaticCommandChannel = 1;

UProgressCommandComponent::UProgressCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	ProgressDuration = 0;
	ProgressIcon = nullptr;
	RemainedProgress = 0;

	Data.bAbortCurrentCommand = false;
	Data.Channel = StaticCommandChannel;
}

void UProgressCommandComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RemainedProgress);
}

float UProgressCommandComponent::GetProgressDuration_Implementation() const
{
	return ProgressDuration;
}

bool UProgressCommandComponent::InternalIsCommandEnable_Implementation(FString& OutDisableReason) const
{
	return GetProgressDuration() >= 0;
}

void UProgressCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();
	RemainedProgress = GetProgressDuration();
}

void UProgressCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	AUTHORITY_CHECK();
	
	RemainedProgress -= DeltaTime;
	if (RemainedProgress <= 0)
	{
		RemainedProgress = 0;
		EndExecute(ECommandExecuteResult::Success);
	}
}
