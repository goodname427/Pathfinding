// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressCommandComponent.h"

#include "PFUtils.h"


UProgressCommandComponent::UProgressCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ProgressDuration = 0;
	RemainedProgress = 0;
}

void UProgressCommandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsExecuting())
	{
		RemainedProgress -= DeltaTime;
		if (RemainedProgress <= 0)
		{
			RemainedProgress = 0;
			EndExecute(ECommandExecuteResult::Success);
		}
	}
}

bool UProgressCommandComponent::InternalIsReachable_Implementation()
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
