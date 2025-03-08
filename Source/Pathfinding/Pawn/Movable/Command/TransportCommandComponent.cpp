// Fill out your copyright notice in the Description page of Project Settings.


#include "TransportCommandComponent.h"

#include "CollectCommandComponent.h"
#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Movable/CollectorPawn.h"

FName UTransportCommandComponent::StaticCommandName = FName("Transport");

UTransportCommandComponent::UTransportCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Data.Name = StaticCommandName;
	Data.bHiddenInCommandListMenu = true;
}

bool UTransportCommandComponent::CanTransport(const ABaseCampPawn* BaseCamp) const
{
	if (BaseCamp == nullptr || BaseCamp->IsPendingKill())
	{
		return false;
	}

	return GetExecutePawn()->GetPawnRole(BaseCamp) == EPawnRole::Self;
}

bool UTransportCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return GetComponentFromExecutePawn<UCollectorComponent>() != nullptr && GetExecutePlayerState() != nullptr;
}

bool UTransportCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	if (const ABaseCampPawn* BaseCampPawn = Request.GetTargetPawn<ABaseCampPawn>())
	{
		if (CanTransport(BaseCampPawn))
		{
			return true;
		}
	}

	return false;
}

void UTransportCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();
	
	ABaseCampPawn* BaseCamp = Request.GetTargetPawn<ABaseCampPawn>();
	UCollectorComponent* Collector = GetComponentFromExecutePawn<UCollectorComponent>();

	if (BaseCamp && Collector)
	{
		BaseCamp->TransportBy(Collector);
		EndExecute(ECommandExecuteResult::Success);
	}
	else
	{
		EndExecute(ECommandExecuteResult::Failed);
	}
}
