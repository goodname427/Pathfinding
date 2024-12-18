// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "Conscious/ConsciousPawn.h"
#include "PFUtils.h"

UCommandComponent::UCommandComponent(): Request()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FName UCommandComponent::GetCommandName_Implementation() const
{
	return NAME_None;
}

float UCommandComponent::GetRequiredTargetRadius_Implementation()
{
	return 10.0f;
}

AConsciousPawn* UCommandComponent::GetExecutePawn() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

AConsciousAIController* UCommandComponent::GetExecuteController() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return ExecutePawn->GetConsciousAIController();
	}

	return nullptr;
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
	if (bExecuting)
	{
		return;
	}
	
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
	if (!bExecuting)
	{
		return;
	}
	
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
