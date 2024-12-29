// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCampPawn.h"

#include "PFUtils.h"


// Sets default values
ABaseCampPawn::ABaseCampPawn()
{
}

void ABaseCampPawn::TransportBy(ACollectorPawn* CollectorPawn)
{
	if (!CollectorPawn->CollectedResource.IsValid())
	{
		return;
	}

	ABattlePlayerState* PS = GetOwnerPlayer();

	PS->AddResource(CollectorPawn->CollectedResource);

	CollectorPawn->CollectedResource.Point = 0;
	CollectorPawn->CollectedResource.Type = EResourceType::None;
}
