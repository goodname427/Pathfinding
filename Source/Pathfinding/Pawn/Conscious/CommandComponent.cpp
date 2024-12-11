// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"

FName UCommandComponent::GetCommandName_Implementation()
{
	return NAME_None;
}

AConsciousPawn* UCommandComponent::GetExecutePawn() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

void UCommandComponent::SetCommandArgs(const FTargetRequest& InRequest)
{
	Request = InRequest;
}

void UCommandComponent::InternalBeginExecute_Implementation()
{
}

bool UCommandComponent::InternalCanExecute_Implementation()
{
	return true;
}

bool UCommandComponent::CanExecute()
{
	return GetExecutePawn() != nullptr && InternalCanExecute();
}

void UCommandComponent::BeginExecute()
{
	bExecuting = true;

	if (OnCommandBegin.IsBound())
	{
		OnCommandBegin.Broadcast(this);
	}
	
	InternalBeginExecute();
}

void UCommandComponent::EndExecute(bool bInSucceeded)
{
	bExecuting = false;
	bSucceeded = bInSucceeded;

	InternalEndExecute();
	
	if (OnCommandEnd.IsBound())
	{
		OnCommandEnd.Broadcast(this);
	}
}

void UCommandComponent::InternalEndExecute_Implementation()
{
}
