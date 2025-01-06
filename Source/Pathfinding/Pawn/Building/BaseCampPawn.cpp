// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCampPawn.h"

#include "PFUtils.h"
#include "Movable/Component/CollectorComponent.h"


// Sets default values
ABaseCampPawn::ABaseCampPawn()
{
}

void ABaseCampPawn::TransportBy(UCollectorComponent* Collector)
{
	if (!Collector->GetCollectedResource().IsValid())
	{
		return;
	}

	ABattlePlayerState* PS = GetOwnerPlayer();

	PS->TakeResource(Collector, EResourceTookReason::Collect, Collector->GetCollectedResource());

	Collector->EmptyCollectedResource();
}
