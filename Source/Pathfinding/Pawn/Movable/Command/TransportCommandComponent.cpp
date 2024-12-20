// Fill out your copyright notice in the Description page of Project Settings.


#include "TransportCommandComponent.h"

#include "CollectCommandComponent.h"
#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Movable/CollectorPawn.h"

FName UTransportCommandComponent::CommandName = FName("Transport");

// Sets default values for this component's properties
UTransportCommandComponent::UTransportCommandComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UTransportCommandComponent::InternalIsReachable_Implementation()
{
	if (GetExecutePawn<ACollectorPawn>() == nullptr)
	{
		return false;
	}
	
	if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(Request.TargetPawn))
	{
		if (BaseCampPawn->GetPawnRole(GetExecutePawn()) == EPawnRole::Self)
		{
			return true;
		}
	}

	return false;
}

void UTransportCommandComponent::InternalBeginExecute_Implementation()
{
	ABaseCampPawn* BaseCamp = Request.GetTargetPawn<ABaseCampPawn>();
	ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();

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

void UTransportCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Success)
	{
		ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();

		FTargetRequest CollectRequest = FTargetRequest::Make<UCollectCommandComponent>();
		{
			CollectRequest.TargetPawn = Collector->GetNextResourceToCollect();
			// DEBUG_MESSAGE(TEXT("Collect Resource [%s]"), *CollectRequest.TargetPawn->GetName());
		}

		Collector->Receive(CollectRequest, false);
	}
}
