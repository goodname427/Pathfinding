// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectCommandComponent.h"

#include "CommanderPawn.h"
#include "PFPawn.h"
#include "TransportCommandComponent.h"
#include "Building/BaseCampPawn.h"
#include "Resource/ResourcePawn.h"

FName UCollectCommandComponent::CommandName = FName("Collect");

// Sets default values for this component's properties
UCollectCommandComponent::UCollectCommandComponent(): ResourceTypesToAllowCollecting({EResourceType::Coin})
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UCollectCommandComponent::CanCollect(const AResourcePawn* ResourcePawn) const
{
	if (ResourcePawn == nullptr)
	{
		return false;
	}

	return ResourceTypesToAllowCollecting.Contains(ResourcePawn->GetResourceType());
}

bool UCollectCommandComponent::InternalIsReachable_Implementation()
{
	if (const AResourcePawn* ResourcePawn = Cast<AResourcePawn>(Request.TargetPawn))
	{
		if (CanCollect(ResourcePawn))
		{
			return true;
		}
	}

	return false;
}

void UCollectCommandComponent::InternalBeginExecute_Implementation()
{
	AResourcePawn* Resource = Cast<AResourcePawn>(Request.TargetPawn);
	ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();

	if (Resource && Collector)
	{
		Resource->CollectBy(Collector);
		EndExecute(ECommandExecuteResult::Success);
	}
	else
	{
		EndExecute(ECommandExecuteResult::Failed);
	}
}

void UCollectCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Success)
	{
		ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();

		Collector->FindAndRecordNextResourceToCollect(Cast<AResourcePawn>(Request.TargetPawn));

		FTargetRequest TransportRequest;
		{
			TransportRequest.CommandName = UTransportCommandComponent::CommandName;
			TransportRequest.TargetPawn = Collector->GetOwnerPlayer()->GetNearestBaseCamp(Collector);
		}

		Collector->Receive(TransportRequest, false);
	}
}
