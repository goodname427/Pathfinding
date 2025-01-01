// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectCommandComponent.h"

#include "CommanderPawn.h"
#include "PFPawn.h"
#include "PFUtils.h"
#include "TransportCommandComponent.h"
#include "Building/BaseCampPawn.h"
#include "Resource/ResourcePawn.h"

FName UCollectCommandComponent::StaticCommandName = FName("Collect");

UCollectCommandComponent::UCollectCommandComponent(): ResourceTypesToAllowCollecting({EResourceType::Coin})
{
	PrimaryComponentTick.bCanEverTick = false;

	Data.Name = StaticCommandName;
}

bool UCollectCommandComponent::CanCollect(const AResourcePawn* ResourcePawn) const
{
	if (ResourcePawn == nullptr || ResourcePawn->IsPendingKill() || ResourcePawn->GetResourcePoint() <= 0)
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
			// DEBUG_MESSAGE(
			// 	TEXT("Collector [%s] Can Collect Resource [%s] [%d]"),
			// 	*GetExecutePawn()->GetName(),
			// 	*ResourcePawn->GetName(),
			// 	ResourcePawn->GetResourcePoint()
			// );
			return true;
		}
	}

	return false;
}

void UCollectCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();

	AResourcePawn* Resource = Request.GetTargetPawn<AResourcePawn>();
	ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();

	if (Resource && Collector && Resource->CollectBy(Collector))
	{
		EndExecute(ECommandExecuteResult::Success);
	}
	else
	{
		EndExecute(ECommandExecuteResult::Failed);
	}
}

void UCollectCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AUTHORITY_CHECK();

	ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();
	if (Result == ECommandExecuteResult::Success)
	{
		Collector->FindAndRecordNextResourceToCollect(Cast<AResourcePawn>(Request.TargetPawn));

		FTargetRequest TransportRequest = FTargetRequest::Make<UTransportCommandComponent>();
		{
			TransportRequest.TargetPawn = Collector->GetOwnerPlayer()->GetNearestBaseCamp(Collector);
			TransportRequest.Type = ETargetRequestType::Append;
		}

		Collector->Receive(TransportRequest);
	}
	else if (Result == ECommandExecuteResult::Failed)
	{
		Collector->FindAndRecordNextResourceToCollect(Cast<AResourcePawn>(Request.TargetPawn));

		FTargetRequest CollectRequest(this, Collector->GetNextResourceToCollect());
		{
			CollectRequest.Type = ETargetRequestType::Append;
		}

		Collector->Receive(CollectRequest);
	}
}

void UCollectCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	AUTHORITY_CHECK();

	// if (Reason == ECommandPoppedReason::Unreachable)
	// {
	// 	ACollectorPawn* Collector = GetExecutePawn<ACollectorPawn>();
	// 	Collector->FindAndRecordNextResourceToCollect(Cast<AResourcePawn>(Request.TargetPawn));
	//
	// 	FTargetRequest CollectRequest(this, Collector->GetNextResourceToCollect());
	// 	{
	// 		CollectRequest.Type = ETargetRequestType::Append;
	// 	}
	//
	// 	Collector->Receive(CollectRequest);
	// }
}
