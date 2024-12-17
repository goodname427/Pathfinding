// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"

FName UCommandComponent::GetCommandName_Implementation() const
{
	return NAME_None;
}

float UCommandComponent::GetRequiredTargetRadius_Implementation()
{
	return -1.0f;
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

	// DEBUG_MESSAGE(TEXT("Conscious Pawn [%s] Execute Command [%s]"), *GetExecutePawn()->GetName(), *GetCommandName().ToString());
	
	InternalBeginExecute();
}

void UCommandComponent::EndExecute(ECommandExecuteResult Result)
{
	bExecuting = false;

	InternalEndExecute(Result);
	
	if (OnCommandEnd.IsBound())
	{
		OnCommandEnd.Broadcast(this, Result);
	}
}

void UCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
}
