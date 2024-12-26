// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectCommandComponent.h"

#include "CommanderPawn.h"
#include "PFPawn.h"
#include "TransportCommandComponent.h"
#include "Building/BaseCampPawn.h"
#include "Resource/ResourcePawn.h"

FName UCollectCommandComponent::StaticCommandName = FName("Collect");

UCollectCommandComponent::UCollectCommandComponent(): ResourceTypesToAllowCollecting({EResourceType::Coin})
{
	PrimaryComponentTick.bCanEverTick = false;

	CommandName = StaticCommandName;	
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
	if (GetExecutePawn<ACollectorPawn>() == nullptr)
	{
		return false;
	}
	
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
	AResourcePawn* Resource = Request.GetTargetPawn<AResourcePawn>();
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

		FTargetRequest TransportRequest = FTargetRequest::Make<UTransportCommandComponent>();
		{
			TransportRequest.TargetPawn = Collector->GetOwnerPlayer()->GetNearestBaseCamp(Collector);
		}

		Collector->Receive(TransportRequest, false);
	}
}
