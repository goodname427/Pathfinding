// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectorPawn.h"

#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Command/CollectCommandComponent.h"
#include "Command/TransportCommandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Resource/ResourcePawn.h"


// Sets default values
ACollectorPawn::ACollectorPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	INIT_DEFAULT_SUBOBJECT(CollectCommandComponent);
	INIT_DEFAULT_SUBOBJECT(TransportCommandComponent);

	// bind command events
	if (HasAuthority())
	{
		CollectCommandComponent->OnCommandEnd.AddDynamic(this, &ACollectorPawn::OnCollectCommandEnd);
		CollectCommandComponent->OnCommandPoppedFromQueue.AddDynamic(
			this, &ACollectorPawn::OnCollectCommandPoppedFromQueue);
		TransportCommandComponent->OnCommandEnd.AddDynamic(this, &ACollectorPawn::OnTransportCommandEnd);
		TransportCommandComponent->OnCommandPoppedFromQueue.AddDynamic(
			this, &ACollectorPawn::OnTransportCommandPoppedFromQueue);
	}

	CollectedResource.Type = EResourceType::None;
	CollectedResource.Point = 0;

	ResourcePointPerCollecting = 2;
	MaxCollectedResourcePoint = 10;
}

void ACollectorPawn::OnReceive_Implementation(const FTargetRequest& Request)
{
	if (Request.Type == ETargetRequestType::StartNew)
	{
		NextResourceToCollect = nullptr;
	}
}

UCommandComponent* ACollectorPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	if (Request.IsTargetPawnValid())
	{
		if (AResourcePawn* ResourcePawn = Request.GetTargetPawn<AResourcePawn>())
		{
			if (CollectCommandComponent->CanCollect(ResourcePawn))
			{
				return CollectCommandComponent;
			}
		}
		else if (ABaseCampPawn* BaseCampPawn = Request.GetTargetPawn<ABaseCampPawn>())
		{
			if (GetPawnRole(BaseCampPawn) == EPawnRole::Self)
			{
				return TransportCommandComponent;
			}
		}
	}

	return MoveCommandComponent;
}

void ACollectorPawn::FindAndRecordNextResourceToCollect(AResourcePawn* CurrentCollectedResource)
{
	NextResourceToCollect = nullptr;
	// continue to collect the same resource
	if (CollectCommandComponent->CanCollect(CurrentCollectedResource))
	{
		NextResourceToCollect = CurrentCollectedResource;
	}
	// if the current resource can not collect, find the nearest resource of the same type
	else
	{
		FVector Location;
		// find the nearest resource for current collected resource
		if (CurrentCollectedResource && !CurrentCollectedResource->IsPendingKill())
		{
			Location = CurrentCollectedResource->GetActorLocation();
		}
		// if not, find the nearest resource for self
		else
		{
			Location = GetActorLocation();
		}

		TArray<AActor*> Resources;
		UGameplayStatics::GetAllActorsOfClass(this, AResourcePawn::StaticClass(), Resources);

		float NearestDistance = 0;
		for (AActor* Actor : Resources)
		{
			if (AResourcePawn* Resource = Cast<AResourcePawn>(Actor))
			{
				if (Resource == CurrentCollectedResource
					|| (CurrentCollectedResource && Resource->GetResourceType() != CurrentCollectedResource->
						GetResourceType()))
				{
					continue;
				}

				const float CurrentDistance = FVector::Dist(Location, Resource->GetActorLocation());
				if (NextResourceToCollect == nullptr || CurrentDistance < NearestDistance)
				{
					NextResourceToCollect = Resource;
					NearestDistance = CurrentDistance;
				}
			}
		}
	}
}

void ACollectorPawn::CollectOrTransportResource(AResourcePawn* CurrentCollectedResource)
{
	FindAndRecordNextResourceToCollect(CurrentCollectedResource);
	if (IsCollectedResourceFull())
	{
		TransportResource();
	}
	else
	{
		if (NextResourceToCollect)
		{
			CollectResource();
		}
		// transport the collected resource when can not find any resource to collect
		else if (!IsCollectedResourceEmpty())
		{
			TransportResource();
		}
	}
}

void ACollectorPawn::CollectResource()
{
	if (!HasAuthority())
	{
		return;
	}

	FTargetRequest CollectRequest(CollectCommandComponent, NextResourceToCollect);
	{
		CollectRequest.Type = ETargetRequestType::Append;
	}

	Receive(CollectRequest);
}

void ACollectorPawn::TransportResource()
{
	if (!HasAuthority())
	{
		return;
	}

	FTargetRequest TransportRequest(TransportCommandComponent, GetOwnerPlayer()->GetNearestBaseCamp(this));
	{
		TransportRequest.Type = ETargetRequestType::Append;
	}

	Receive(TransportRequest);
}

void ACollectorPawn::OnCollectCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Aborted)
	{
		return;
	}

	CollectOrTransportResource(CommandComponent->GetRequest().GetTargetPawn<AResourcePawn>());
}

void ACollectorPawn::OnCollectCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason)
{
	if (Reason == ECommandPoppedReason::Cancel)
	{
		return;
	}

	CollectOrTransportResource(CommandComponent->GetRequest().GetTargetPawn<AResourcePawn>());
}

void ACollectorPawn::OnTransportCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Success)
	{
		// if the marked resource has been empty, find the nearest resource to collect
		// else, continue to collect the same resource
		if (NextResourceToCollect != nullptr)
		{
			FindAndRecordNextResourceToCollect(NextResourceToCollect);
		}
		CollectResource();
	}
	else if (Result == ECommandExecuteResult::Failed)
	{
		TransportResource();
	}
}

void ACollectorPawn::OnTransportCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason)
{
	if (Reason == ECommandPoppedReason::CanNotExecute || Reason == ECommandPoppedReason::PreTaskFailed)
	{
		TransportResource();
	}
}

void ACollectorPawn::SetCollectedResourceType(EResourceType NewResourceType)
{
	if (NewResourceType != CollectedResource.Type)
	{
		CollectedResource.Point = 0;
	}

	CollectedResource.Type = NewResourceType;
}
