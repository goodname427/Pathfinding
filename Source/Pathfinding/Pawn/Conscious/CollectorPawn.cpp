// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectorPawn.h"

#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Command/CollectCommandComponent.h"
#include "Command/TransportCommandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Resource/ResourcePawn.h"


// Sets default values
ACollectorPawn::ACollectorPawn(): CollectedResource(0), CollectedResourceType(EResourceType::None)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	INIT_DEFAULT_SUBOBJECT(CollectCommandComponent);
	INIT_DEFAULT_SUBOBJECT(TransportCommandComponent);
}

UCommandComponent* ACollectorPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	if (Request.TargetPawn != nullptr)
	{
		if (AResourcePawn* ResourcePawn = Cast<AResourcePawn>(Request.TargetPawn))
		{
			if (CollectCommandComponent->CanCollect(ResourcePawn))
			{
				return CollectCommandComponent;
			}
		}
		else if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(Request.TargetPawn))
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
	if (CurrentCollectedResource)
	{
		if (CurrentCollectedResource->GetResourcePoint() > 0)
		{
			NextResourceToCollect = CurrentCollectedResource;
		}
		else
		{
			TArray<AActor*> Resources;
			UGameplayStatics::GetAllActorsOfClass(this, AResourcePawn::StaticClass(), Resources);

			const FVector Location = CurrentCollectedResource->GetActorLocation();
			float Distance = 0;
			for (AActor* Actor : Resources)
			{
				if (AResourcePawn* Resource = Cast<AResourcePawn>(Actor))
				{
					if (Resource == CurrentCollectedResource || Resource->GetResourceType() != CurrentCollectedResource->GetResourceType())
					{
						continue;
					}
					
					float CurrentDistance = FVector::Dist(Location, Resource->GetActorLocation());
					if (NextResourceToCollect == nullptr || CurrentDistance < Distance)
					{
						NextResourceToCollect = Resource;
						Distance = CurrentDistance;
					}
				}
			}
		}
	}
}
