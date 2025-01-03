﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressCommandComponent.h"

#include "PFUtils.h"

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

bool UProgressCommandComponent::InternalIsArgumentsValid_Implementation()
{
	return GetProgressDuration() >= 0;
}

float UProgressCommandComponent::GetProgressDuration_Implementation() const
{
	return ProgressDuration;
}

void UProgressCommandComponent::InternalBeginExecute_Implementation()
{
	RemainedProgress = GetProgressDuration();
}

void UProgressCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	RemainedProgress -= DeltaTime;
	if (RemainedProgress <= 0)
	{
		RemainedProgress = 0;
		END_EXECUTE_AUTHORITY(ECommandExecuteResult::Success);
	}
}
