// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectorComponent.h"

#include "DelayHelper.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Movable/Command/CollectCommandComponent.h"
#include "Movable/Command/TransportCommandComponent.h"
#include "Building/BaseCampPawn.h"


// Sets default values for this component's properties
UCollectorComponent::UCollectorComponent(): CollectCommandComponent(nullptr), TransportCommandComponent(nullptr),
                                            Collector(nullptr),
                                            ResourcePointPerCollecting(2),
                                            MaxCollectedResourcePoint(10),
                                            NextResourceToCollect(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCollectorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Collector == nullptr)
	{
		Collector = GetOwner<AConsciousPawn>();
	}

	if (Collector)
	{
		if (CollectCommandComponent == nullptr)
		{
			CollectCommandComponent = Cast<UCollectCommandComponent>(
				Collector->GetComponentByClass(UCollectCommandComponent::StaticClass()));
		}
		if (TransportCommandComponent == nullptr)
		{
			TransportCommandComponent = Cast<UTransportCommandComponent>(
				Collector->GetComponentByClass(UTransportCommandComponent::StaticClass()));
		}
	}


	if (CollectCommandComponent == nullptr || TransportCommandComponent == nullptr || Collector == nullptr)
	{
		// DestroyComponent();
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		Collector->OnReceivedRequest.AddDynamic(this, &ThisClass::OnReceivedRequest);
		
		CollectCommandComponent->OnCommandEnd.AddDynamic(this, &ThisClass::OnCollectCommandEnd);
		CollectCommandComponent->OnCommandPoppedFromQueue.AddDynamic(
			this, &ThisClass::OnCollectCommandPoppedFromQueue);
		TransportCommandComponent->OnCommandEnd.AddDynamic(this, &ThisClass::OnTransportCommandEnd);
		TransportCommandComponent->OnCommandPoppedFromQueue.AddDynamic(
			this, &ThisClass::OnTransportCommandPoppedFromQueue);
	}
	// else
	// {
	// 	DestroyComponent();
	// 	return;
	// }
}


void UCollectorComponent::SetCollectedResourceType(EResourceType NewResourceType)
{
	if (NewResourceType != CollectedResource.Type)
	{
		CollectedResource.Point = 0;
	}

	CollectedResource.Type = NewResourceType;
}

void UCollectorComponent::OnReceivedRequest(AConsciousPawn* ConsciousPawn, const FTargetRequest& Request)
{
	if (Request.Type == ETargetRequestType::StartNew)
	{
		NextResourceToCollect = nullptr;
	}
}

void UCollectorComponent::FindAndRecordNextResourceToCollect(AResourcePawn* CurrentCollectedResource)
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
			Location = Collector->GetActorLocation();
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

void UCollectorComponent::CollectOrTransportResource(AResourcePawn* CurrentCollectedResource)
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

void UCollectorComponent::CollectResource()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}

	FTargetRequest CollectRequest(CollectCommandComponent, NextResourceToCollect);
	{
		CollectRequest.Type = ETargetRequestType::Append;
	}

	Collector->Receive(CollectRequest);
}

void UCollectorComponent::TransportResource(bool bLastTransposingFailed)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}
	
	ABaseCampPawn* BaseCamp = Collector->GetOwnerPlayer()->GetNearestBaseCamp(Collector);
	
	FTargetRequest TransportRequest(TransportCommandComponent, BaseCamp);
	{
		TransportRequest.Type = ETargetRequestType::Append;
	}

	auto Receive = [this, TransportRequest]{ Collector->Receive(TransportRequest); };
	
	if (bLastTransposingFailed)
	{
		// Delay the transport request to prevent stack overflow caused by recursive calling.
		Delay(this, 0.1f, Receive);
		// UDelayHelper::Delay(this, 0.1f, Receive);
	}
	else
	{
		Receive();
	}
}

void UCollectorComponent::OnCollectCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Aborted)
	{
		return;
	}

	CollectOrTransportResource(CommandComponent->GetRequest().GetTargetPawn<AResourcePawn>());
}

void UCollectorComponent::OnCollectCommandPoppedFromQueue(UCommandComponent* CommandComponent,
                                                          ECommandPoppedReason Reason)
{
	if (Reason == ECommandPoppedReason::Cancel)
	{
		return;
	}

	CollectOrTransportResource(CommandComponent->GetRequest().GetTargetPawn<AResourcePawn>());
}

void UCollectorComponent::OnTransportCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
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
		TransportResource(true);
	}
}

void UCollectorComponent::OnTransportCommandPoppedFromQueue(UCommandComponent* CommandComponent,
                                                            ECommandPoppedReason Reason)
{
	if (Reason == ECommandPoppedReason::CanNotExecute || Reason == ECommandPoppedReason::PreTaskFailed)
	{
		TransportResource(true);
	}
}
