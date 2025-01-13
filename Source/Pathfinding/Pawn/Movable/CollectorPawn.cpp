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
	INIT_DEFAULT_SUBOBJECT(CollectCommandComponent);
	INIT_DEFAULT_SUBOBJECT(TransportCommandComponent);
	INIT_DEFAULT_SUBOBJECT(CollectorComponent);
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
			if (TransportCommandComponent->CanTransport(BaseCampPawn))
			{
				return TransportCommandComponent;
			}
		}
	}

	return MoveCommandComponent;
}


